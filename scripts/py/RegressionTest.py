"""
Self contained regression test
"""
import os, sys
import difflib
from subprocess import Popen, PIPE

class TestOutput:
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

class Message:
    text = None
    diff = None

    def __init__(self, text, expected=None, actual=None):
        self.text = text

        # Perform a diff if requested
        if expected != None and actual != None:
            differ = difflib.Differ()
            difflines = differ.compare(expected.splitlines(), actual.splitlines())
            difftext = '\n'.join(difflines)
            self.diff = f'DIFF:\n{difftext}\nEXPECTED:\n{expected}\nACTUAL:\n{actual}\n'

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

class Test:
    exeArgs = None
    exeArgsText = None
    inputData = None
    expected = None
    verbose = False
    command = None

    """Initialize the test"""
    def __init__(self, exePath, scriptPath):
        # Make sure the test script exists.
        if not os.path.isfile(scriptPath):
            sys.exit(f'Path to regression test script is not valid: {scriptPath}')

        self.exePath = exePath
        self.path = scriptPath
        self.scriptName = os.path.basename(self.path)
        self.dir = os.path.dirname(self.path)
        self.name = os.path.splitext(self.scriptName)[0]

    """Executes the test"""
    def run(self):
        if self.verbose:
            print(f'RUN TEST: {self.name}')

        self.loadTestData()
        return self.runTestExe()

    """Load test parameters using the name of the regression test but with different extensions"""
    def loadTestData(self):
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
        filePath = os.path.join(self.dir, self.scriptName.replace('.bf', f'.{dataExtension}'))
        fileData = open(filePath, 'rb').read() if os.path.exists(filePath) else b''

        if self.verbose and fileData != b'':
            print(f'Got .{dataExtension}')

        return fileData

    """Run the test using the user provided intepreter executable. Make sure to redirect any input streamas that were
    specified for the test."""
    def runTestExe(self):
        self.command = f'{self.exePath} {self.path} {self.exeArgsText}'

        if (self.verbose):
            print(f'Invoke: `{self.command}``')

        process = Popen(self.command, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

        process.stdin.write(self.inputData)
        process.stdin.close()

        actual = TestOutput(process.stdout.read(), process.stderr.read(), process.wait())
        return Result(self, self.expected, actual)
