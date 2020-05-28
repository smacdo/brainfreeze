"""
A simple framework for running regression tests on console applications and reporting their results.
"""
import os, sys, glob, struct
import difflib
from subprocess import Popen, PIPE

class TestOutput:
    """Holds the output (expected or actual) from a test scenario."""
    stdout = None
    stderr = None
    exitCode = None

    def __init__(self, stdout, stderr, exitCode):
        self.stdout = stdout
        self.stderr = stderr
        self.exitCode = exitCode

    def hasExitCode(self):
        return self.exitCode != None and self.exitCode != b''

    def isSuccessfulExitCode(self):
        return self.hasExitCode and self.exitCode == 0

class TestScenariosFinder:
    """Locates scenarios to be tested according to configurable options"""
    dirs = []
    shouldSort = False

    def __init__(self, dirs, shouldSort=False):
        self.dirs = dirs
        self.shouldSort = shouldSort

    def findAll(self):
        """Find all tests"""
        scenarios = []

        if self.shouldSort:
            self.dirs.sort()

        for testDir in self.dirs:
            if not os.path.isdir(testDir):
                sys.exit(f'Directory not found: {testDir}')

            searchPattern = os.path.join(testDir, '*.bf')
            scenarioPaths = glob.glob(searchPattern)

            if (self.shouldSort):
                scenarioPaths.sort()

            for scenarioPath in scenarioPaths:
                scenario = TestScenario(scenarioPath)
                scenarios.append(scenario)
        
        return scenarios

class TestRunner:
    """Runs multiple test scenarios according to configuration options"""

    def __init__(self, executor=None, scenarios=[], output=None, ignoreFailures=False):
        self.executor = executor
        self.scenarios = scenarios
        self.output = output
        self.ignoreFailures = ignoreFailures

    def run(self):
        self._onRunStart()
        failCount = 0

        for test in self.scenarios:
            self._onTestStart(test)
            result = self.executor.run(test)

            if result.failed:
                failCount = failCount + 1
                self._onTestFail(test, result)

                if not self.ignoreFailures:
                    break
            else:
                self._onTestOk(test, result)

        self._onFinish()
        return failCount == 0

    def _onRunStart(self):
        if self.output is not None:
            self.output.onRunStart(len(self.scenarios))

    def _onTestStart(self, test):
        if self.output is not None:
            self.output.onTestStart(test)

    def _onTestFail(self, test, result):
        if self.output is not None:
            self.output.onTestFail(test, result)

    def _onTestOk(self, test, result):
        if self.output is not None:
            self.output.onTestOk(test, result)

    def _onFinish(self):
        if self.output is not None:
            self.output.onFinish()

class TestScenario:
    """Holds information related to the running of a single test scenario / test case"""
    exeArgs = None
    exeArgsText = None
    inputData = None
    expected = None
    verbose = False
    command = None

    def __init__(self, scriptPath):
        # Make sure the test script exists.
        if not os.path.isfile(scriptPath):
            sys.exit(f'Path to regression test script is not valid: {scriptPath}')

        self.path = scriptPath
        self.scriptName = os.path.basename(self.path)
        self.dir = os.path.dirname(self.path)
        self.name = os.path.splitext(self.scriptName)[0]

        # Load expected input/output values from disk (if provided).
        if self.exeArgs == None:
            self.exeArgs = self.readTestDataFile('args')
            self.exeArgsText = self.exeArgs.decode('utf-8').rstrip()

        if self.inputData == None:
            self.inputData = self.readTestDataFile('stdin')

        if self.expected == None:
            self.expected = TestOutput(
                self.readTestDataFile('stdout'),
                self.readTestDataFile('stderr'),
                self.readTestDataFile('exitcode'))

    def readTestDataFile(self, dataExtension):
        """Load test data from disk"""
        filePath = os.path.join(self.dir, self.scriptName.replace('.bf', f'.{dataExtension}'))
        fileData = open(filePath, 'rb').read() if os.path.exists(filePath) else b''

        if self.verbose and fileData != b'':
            print(f'Got .{dataExtension}')

        return fileData

class TestExecutor:
    """Executes the program under test with a scenario and returns the result of running it"""
    verbose : bool = False
    exePath : str = None

    def __init__(self, exePath: str, verbose: bool=False):
        if not os.path.isfile(exePath):
            sys.exit("Could not find path to test exe")

        self.exePath = exePath
        self.verbose = verbose

    def run(self, test):
        # Create invocation command.
        command = f'{self.exePath} {test.path} {test.exeArgsText}'

        if (self.verbose):
            print(f'Invoke: `{command}``')

        process = Popen(command, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

        process.stdin.write(test.inputData)
        process.stdin.close()

        actual = TestOutput(process.stdout.read(), process.stderr.read(), process.wait())
        return Result(test, test.expected, actual)

class Message:
    """Holds a test result message with optional diff text to display"""
    text = None
    diff = None

    def __init__(self, text, expected=None, actual=None):
        self.text = text
        self.expected = expected
        self.actual = actual

        # Perform a diff if requested
        if expected != None and actual != None:
            differ = difflib.Differ()
            difflines = differ.compare(expected.splitlines(), actual.splitlines())
            self.diff = '\n'.join(difflines)
            #self.diff = f'EXPECTED:\n{expected}\nACTUAL:\n{actual}\nDIFF:\n{difftext}\n'

    def hasDiff(self):
        return self.diff != None

class Result:
    messages = []
    failed = False

    def __init__(self, test, expected, actual):
        self.expected = expected
        self.actual = actual
        self.name = test.name
        self.command = test.command

        if self.expected.hasExitCode() and self.expected.exitCode != self.actual.exitCode:
            self.addFailure(f'Expected exit code {self.expected.exitCode} but was {self.actual.exitCode}')
        elif self.actual.hasExitCode() and not self.actual.isSuccessfulExitCode():
            self.addFailure(f'Expected successful exit code but was {self.actual.exitCode}')

        if self.expected.stdout != self.actual.stdout:
            self.addFailureDiff('Output did not match expected output', self.expected.stdout, self.actual.stdout)

        if self.expected.stderr != self.actual.stderr:
            self.addFailureDiff('Error did not match expected error', self.expected.stderr, self.actual.stderr)

    def addFailure(self, text):
        self._addFailure(text, None, None)

    def addFailureDiff(self, text, expected, actual):
        self._addFailure(text, expected.decode('utf-8'), actual.decode('utf-8'))

    def _addFailure(self, text, expected, actual):
        self.failed = True
        self.messages.append(Message(text, expected, actual))

class OutputHandler:
    """Subclass to provide custom output from test runner"""

    def onRunStart(self, totalTests):
        """Called when test runner about to run tests"""
        pass

    def onTestStart(self, scenario):
        """Called when a test scenario is about to be executed"""
        pass

    def onTestOk(self, scenario, result):
        """Called when test scenario runs succesfully"""
        pass

    def onTestFail(self, scenario, result):
        """Called when test scenario fails"""
        pass

    def onFinish(self):
        """Called when test runner finishes running"""
        pass

class MultiOutputHandler(OutputHandler):
    """Multiplexes event to multiple output handlers"""

    def __init__(self, handlers=[]):
        self.handlers = handlers

    def add(self, handler):
        self.handlers.append(handler)

    def onRunStart(self, totalTests):
        for h in self.handlers:
            h.onRunStart(totalTests)

    def onTestStart(self, scenario):
        for h in self.handlers:
            h.onTestStart(scenario)

    def onTestOk(self, scenario, result):
        for h in self.handlers:
            h.onTestOk(scenario, result)

    def onTestFail(self, scenario, result):
        for h in self.handlers:
            h.onTestFail(scenario, result)

    def onFinish(self):
        for h in self.handlers:
            h.onFinish()

class PosthocOutputHandler(OutputHandler):
    """Writes failed test output to an output directory for posthoc debugging"""
    def __init__(self, outputDir, verbose=False):
        self.outputDir = outputDir
        self.verbose = verbose

    def onTestFail(self, scenario, result):
        # Create output directory if it doesn't already exist
        if not os.path.exists(self.outputDir):
            if self.verbose:
                print(f'Output directory does not exist, creating: {self.outputDir}')
            os.mkdir(self.outputDir)

        # Write out streams that have content in them
        if result.actual.stdout != b'':
            open(os.path.join(self.outputDir, f'{result.name}.stdout'), 'wb').write(result.actual.stdout)
        if result.actual.stderr != b'':
            open(os.path.join(self.outputDir, f'{result.name}.stderr'), 'wb').write(result.actual.stderr)
        if result.actual.exitCode != b'':
            open(os.path.join(self.outputDir, f'{result.name}.exitcode'), 'wb').write(struct.pack('i', result.actual.exitCode))

        if self.verbose:
            print(f'Wrote failure details to: {self.outputDir}')

class ConsoleOutputHandler(OutputHandler):
    """Prints test runner output to the console"""
    testCounter = 0
    failCounter = 0
    okCounter = 0

    def __init__(self, useColors=False):
        self.redColorStr = '\033[0;31m' if useColors else ''
        self.greenColorStr = '\033[0;32m' if useColors else ''
        self.resetColorStr = '\033[0m' if useColors else ''

    def onRunStart(self, totalTests):
        """Called when test runner about to run tests"""
        self.totalTests = totalTests
        print(f'Running {self.totalTests} regression tests...')

    def onTestStart(self, scenario):
        """Called when a test scenario is about to be executed"""
        self.testCounter = self.testCounter + 1
        print(f'[{self.testCounter}/{self.totalTests}] {scenario.name}')
        pass
    
    def onTestOk(self, scenario, result):
        """Prints test runner output to the console"""
        self.okCounter = self.okCounter + 1
        pass

    def onTestFail(self, scenario, result):
        """Called when test scenario fails"""
        self.failCounter = self.failCounter + 1
        print(f'{self.redColorStr}FAILED: {result.command}{self.resetColorStr}')

        for message in result.messages:
            print(f'{self.redColorStr}{message.text}{self.resetColorStr}')
            # TODO: Use console formatting to make this look nicer.
            if message.expected is not None:
                print("EXPECTED:")
                print(message.expected)

            if message.actual is not None:
                print("ACTUAL:")
                print(message.actual)

            if message.diff is not None:
                print("DIFF:")
                print(message.diff)

    def onFinish(self):
        if self.failCounter > 0:
            print(self.redColorStr, end='')
        else:
            print(self.greenColorStr, end='')

        print(f'Ran {self.testCounter} tests.', end='')
        print(f'{self.okCounter} ok, {self.failCounter} failed, {self.totalTests} total', end='')
        print(self.resetColorStr)