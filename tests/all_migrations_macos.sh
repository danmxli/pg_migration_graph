#!/bin/bash
# chmod +x tests/all_migrations.sh

################################################################################
# run ./tests/all_migrations_macos.sh to run all tests
# run ./tests/all_migrations_macos.sh --leak-check <file> to check for leaks in a specific file
# run ./tests/all_migrations_macos.sh --check-all-leaks to check for leaks in all files
################################################################################

# Path to the executable (relative to project root)
EXECUTABLE="./pg_migration_graph.out"

# Check if the executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found at $EXECUTABLE"
    echo "Make sure to build the project first with 'make -f dev.mk'"
    exit 1
fi

# Check if leaks command exists (macOS only)
if ! command -v leaks &> /dev/null; then
    echo "Error: 'leaks' command not found. This script requires macOS."
    exit 1
fi

# Helper function to check memory leaks for a specific SQL statement
check_leaks() {
    SQL="$1"
    echo "Testing for memory leaks..."
    
    # Use MallocStackLogging and leaks --atExit for proper leak detection
    MallocStackLogging=1 leaks --atExit -- $EXECUTABLE "$SQL"
    LEAK_STATUS=$?
    
    if [ $LEAK_STATUS -eq 0 ]; then
        echo "✅ No memory leaks detected"
    else
        echo "❌ Memory leaks detected"
    fi
    
    return $LEAK_STATUS
}

# Find all SQL files in the tests directory and subdirectories
SQL_FILES=$(find ./tests -type f -name "*.sql")

if [ -z "$SQL_FILES" ]; then
    echo "No SQL files found in tests directory and subdirectories"
    exit 1
fi

# Process each SQL file
for SQL_FILE in $SQL_FILES; do
    echo "======================================================="
    echo "Testing file: $SQL_FILE"
    echo "======================================================="
    
    # Read the SQL file content
    SQL_CONTENT=$(cat "$SQL_FILE")
    
    # Run the executable directly
    $EXECUTABLE "$SQL_CONTENT"
    EXIT_CODE=$?
    
    # Report command status
    if [ $EXIT_CODE -eq 0 ]; then
        echo "✅ Test passed: $SQL_FILE"
    else
        echo "❌ Test failed: $SQL_FILE (exit code: $EXIT_CODE)"
    fi
    
    echo ""
done

echo "All tests completed."

# Check memory leaks for a specific file if requested
if [ "$1" == "--leak-check" ] && [ -n "$2" ]; then
    if [ -f "$2" ]; then
        echo "Running memory leak check for $2"
        check_leaks "$(cat "$2")"
    else
        echo "Error: File $2 not found"
        exit 1
    fi
fi

# Check memory leaks for all files if requested
if [ "$1" == "--check-all-leaks" ]; then
    echo "Running memory leak checks for all files..."
    for SQL_FILE in $SQL_FILES; do
        echo "======================================================="
        echo "Memory leak check: $SQL_FILE"
        echo "======================================================="
        check_leaks "$(cat "$SQL_FILE")"
        echo ""
    done
fi