"""
Takes a directory of programming scripts, passing command line arguments and optional stdin (specified as
scriptname.stdin). This regression test will execute each programming script and capture its standard
output, standard error and exit status code which will be compared to expected values (*.stdout, stderr, code).

Once completed the regression test will print status of tests.
"""
import os, sys, glob, time, struct, argparse
from subprocess import Popen, PIPE

# TODO: Investigate improve security by running test execution in sandboxed environment
# TODO: Get option to be silent?
# TODO: Check that python isn't silently changing newlines when reading from file and that the \r\n isn't showing
#       up in test output

#######################################################################################################################
# Functions
#######################################################################################################################
def readTestDataFile(testDir, testName, dataExtension):
    filePath = os.path.join(testDir, testName.replace('.bf', f'.{dataExtension}'))
    fileData = open(filePath, 'rb').read() if os.path.exists(filePath) else b''

    if Verbose and fileData != b'':
        print(f'Found .{dataExtension}')

    return fileData

def writeFailure(outDir, testName, stdoutResult, stderrResult, exitCode):
    # Create output directory if it doesn't already exist
    if not os.path.exists(outDir):
        if Verbose:
            print(f'Output directory does not exist, creating: {outDir}')
        os.mkdir(outDir)

    # Write out streams that have content in them
    if stdoutResult != b'':
        open(os.path.join(outDir, testName.replace('.bf', '.stdout')), 'wb').write(stdoutResult)
    if stderrResult != b'':
        open(os.path.join(outDir, testName.replace('.bf', '.stderr')), 'wb').write(stderrResult)
    if exitCode != b'':
        open(os.path.join(outDir, testName.replace('.bf', '.result')), 'wb').write(struct.pack('i', exitCode))

    if Verbose:
        print(f'Wrote failure details to: {outDir}')

#######################################################################################################################
# Regression test script
#######################################################################################################################
# Configuration
parser = argparse.ArgumentParser(description='Run suite of regression tests and reports results')
parser.add_argument('exepath', help='Path to language interpreter')
parser.add_argument('testdir', help='Path to directory containing regression tests')
parser.add_argument('--verbose', default=False, action='store_true', help='Print more information out while running tests')
parser.add_argument('--outdir', default=None, help='Directory that will receive failed test run results')

args = parser.parse_args()

Verbose = args.verbose
TestDir = args.testdir
InterpreterExePath = args.exepath
OutputDir = args.outdir

# Make sure interpreter exe path is valid
if not os.path.isfile(InterpreterExePath):
    sys.exit("Could not find interpreter exe")

# Gather scripts to be tested.
if not os.path.isdir(args.testdir):
    sys.exit("Could not find test directory")

testPattern = os.path.join(args.testdir, '*.bf')
testFiles = glob.glob(testPattern)
testFiles.sort()

numberTotal = 0
numberFailed = 0
numberSuccess = 0

# Regression test all found scripts
for testFilePath in testFiles:
    testName = os.path.basename(testFilePath)
    print(f'[{numberTotal + 1}/{len(testFiles)}] {testName}')

    # Get command line args, standard input, output and error if present
    argsData = readTestDataFile(TestDir, testName, 'args')
    stdinData = readTestDataFile(TestDir, testName, 'stdin')
    stdoutData = readTestDataFile(TestDir, testName, 'stdout')
    stderrData = readTestDataFile(TestDir, testName, 'stderr')
    resultData = readTestDataFile(TestDir, testName, 'result')

    resultCode = int(resultData, 2) if resultData != b'' else 0

    # Run the test using the user provided intepreter executable. Make sure to redirect any input streamas that were
    # specified for the test.
    testCommand = f'{InterpreterExePath} {testFilePath} {argsData.decode("utf-8")}'

    if (Verbose):
        print(f'Test invocation: "{testCommand}"')

    process = Popen(testCommand, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)

    process.stdin.write(stdinData)
    process.stdin.close()

    stdoutResult = process.stdout.read()
    stderrResult = process.stderr.read()
    exitResult = process.wait()

    # Check if process succeeded or failed. If there was a provided expected result then also make sure that matches.
    testOk = True

    if resultData == '':
        if exitResult != 0:
            print(f'FAIL: Test exited with {exitResult}')
            testOk = False
    else:
        if resultCode != exitResult:
            print(f'FAIL: Expected exit code {resultCode} but was {exitResult}')
            testOk = False

    # Check if output matched expected values.
    if stdoutResult != stdoutData:
        print(f'FAIL: Standard output did not match expected value')
        testOk = False

    if stderrResult != stderrData:
        print(f'FAIL: Standard error did not match expected value')
        testOk = False

    # If test failed then write results to output directory for post-hoc debugging
    numberTotal = numberTotal + 1

    if testOk:
        numberSuccess = numberSuccess + 1
    else:
        
        numberFailed = numberFailed + 1
        if OutputDir != None:
            writeFailure(OutputDir, testName, stdoutResult, stderrResult, exitResult)

# Write results of running tests
# TODO: Print more detailed results
print(f'{numberSuccess}/{numberSuccess + numberFailed} tests passed')

if numberFailed > 0:
    print(f'*** Tests failed! Output from failed tests written to: {OutputDir} ***')

# Return OK only if all tests passed
if testOk:
    sys.exit(0)
else:
    sys.exit(1)