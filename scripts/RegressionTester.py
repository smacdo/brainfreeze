"""
Takes a directory of programming scripts, passing command line arguments and optional stdin (specified as
scriptname.stdin). This regression test will execute each programming script and capture its standard
output, standard error and exit status code which will be compared to expected values (*.stdout, stderr, code).

Once completed the regression test will print status of tests.
"""
import os, sys, glob, time, struct, argparse
from subprocess import Popen, PIPE

from py import RegressionTest as RT

# TODO: Investigate improve security by running test execution in sandboxed environment
# TODO: Get option to be silent?
# TODO: Check that python isn't silently changing newlines when reading from file and that the \r\n isn't showing
#       up in test output
# TODO: Switch .result to .exitcode

#######################################################################################################################
# Functions
#######################################################################################################################
def readTestDataFile(testDir, testName, dataExtension):
    filePath = os.path.join(testDir, testName.replace('.bf', f'.{dataExtension}'))
    fileData = open(filePath, 'rb').read() if os.path.exists(filePath) else b''

    if Verbose and fileData != b'':
        print(f'Found .{dataExtension}')

    return fileData

def writeFailure(outDir, result):
    # Create output directory if it doesn't already exist
    if not os.path.exists(outDir):
        if Verbose:
            print(f'Output directory does not exist, creating: {outDir}')
        os.mkdir(outDir)

    # Write out streams that have content in them
    if result.actual.stdout != b'':
        open(os.path.join(outDir, f'{result.name}.stdout'), 'wb').write(result.actual.stdout)
    if result.actual.stderr != b'':
        open(os.path.join(outDir, f'{result.name}.stderr'), 'wb').write(result.actual.stderr)
    if result.actual.exitCode != b'':
        open(os.path.join(outDir, f'{result.name}.result'), 'wb').write(struct.pack('i', result.actual.exitCode))

    if Verbose:
        print(f'Wrote failure details to: {outDir}')

#######################################################################################################################
# Regression test script
#######################################################################################################################
# Configuration
# TODO: Detect if console supports colors and set default on that
parser = argparse.ArgumentParser(description='Runs suite of regression tests and reports results')
parser.add_argument('testdir', help='Path to directory containing regression tests')
parser.add_argument('-e', '--exe', help='Path to language interpreter')
parser.add_argument('-v', '--verbose', default=False, action='store_true', help='Print more information out while running tests')
parser.add_argument('-o', '--outdir', default=None, help='Directory that will receive failed test run results')
parser.add_argument('--ignoreFailures', default=False, help='If test runner should continue even if a test fails')
parser.add_argument('--color', default=True, help='Use text color formatting')

args = parser.parse_args()

Verbose = args.verbose
TestDir = args.testdir
InterpreterExePath = args.exe
OutputDir = args.outdir
IgnoreFailures = args.ignoreFailures
UseColors = args.color

# Create text color options
RedColorStr = '\033[0;31m' if UseColors else ''
GreenColorStr = '\033[0;32m' if UseColors else ''
ResetColorStr = '\033[0m' if UseColors else ''

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

# Construct regression test objects for each found test.
Tests = []

for testFilePath in testFiles:
    test = RT.Test(InterpreterExePath, testFilePath)
    Tests.append(test)

# Run all tests
print(f'Running {len(Tests)} regression tests...')
testCount = 0

for test in Tests:
    # Run the test
    print(f'[{testCount}/{len(Tests)}] {test.name}')
    result = test.run()

    testCount = testCount + 1

    if result.failed:
        numberFailed = numberFailed + 1

        print(f'{RedColorStr}FAILED: {result.command}{ResetColorStr}')

        for message in result.messages:
            print(f'{RedColorStr}{message.text}{ResetColorStr}')
            if message.hasDiff():
                print(message.diff)

        # If test failed then write results to output directory for post-hoc debugging
        writeFailure(OutputDir, result)

        if not IgnoreFailures:
            sys.exit('Exiting because of test failure')
    else:
        numberSuccess = numberSuccess + 1

# Write results of running tests
# TODO: Print more detailed results
print(f'{numberSuccess}/{numberSuccess + numberFailed} tests passed')

if numberFailed > 0:
    print(f'*** {RedColorStr}Tests failed!{ResetColorStr} Output from failed tests written to: {OutputDir} ***')
    sys.exit(1)
else:
    sys.exit(0)
