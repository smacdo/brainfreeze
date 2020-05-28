"""
Takes a directory of programming scripts, passing command line arguments and optional stdin (specified as
scriptname.stdin). This regression test will execute each programming script and capture its standard
output, standard error and exit status code which will be compared to expected values (*.stdout, stderr, code).

Once completed the regression test will print status of tests.
"""
import sys, argparse
from py import RegressionTest as RT  # TODO: Improve packaging and namespacing. Break some classes into other files?

# TODO: Investigate improve security by running test execution in sandboxed environment
# TODO: Get option to be silent?
# TODO: Check that python isn't silently changing newlines when reading from file and that the \r\n isn't showing
#       up in test output
# TODO: Run multiple instances of the test program to improve speed
# TODO: Add benchmark support
# TODO: Add multiple regression directories.
# TODO: Add xunit output format for CI
# TODO: Detect if console supports colors and set default on that

#######################################################################################################################
# Regression test script
#######################################################################################################################
# Configuration
parser = argparse.ArgumentParser(description='Runs suite of regression tests and reports results')
parser.add_argument('testdir', help='Path to directory containing regression tests')
parser.add_argument('-e', '--exe', help='Path to language interpreter')
parser.add_argument('-v', '--verbose', default=False, action='store_true', help='Print more information out while running tests')
parser.add_argument('-o', '--outdir', default=None, help='Directory that will receive failed test run results')
parser.add_argument('--ignoreFailures', default=False, action='store_true', help='If test runner should continue even if a test fails')
parser.add_argument('--color', default=True, help='Use text color formatting')

args = parser.parse_args()

Verbose = args.verbose
TestDir = args.testdir
InterpreterExePath = args.exe
OutputDir = args.outdir
IgnoreFailures = args.ignoreFailures
UseColors = args.color

# Construct program to test.
tester = RT.TestExecutor(exePath=InterpreterExePath, verbose=Verbose)

# Find all relevant test scenarios.
searcher = RT.TestScenariosFinder([TestDir], shouldSort=True)
scenarios = searcher.findAll()

# Run all tests and report results.
outputs = RT.MultiOutputHandler()
outputs.add(RT.ConsoleOutputHandler(useColors=UseColors))

if OutputDir is not None:
    outputs.add(RT.PosthocOutputHandler(outputDir=OutputDir, verbose=Verbose))

runner = RT.TestRunner(tester, scenarios, outputs, ignoreFailures=IgnoreFailures)
result = runner.run()

if not result:
    sys.exit(1)
