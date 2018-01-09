file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)
add_custom_target(
        linter
        COMMAND /usr/local/bin/cpplint
        --filter=-,+build/include,-build/include_order,-build/include_what_you_use,+build/storage_class,+readability/alt_tokens,+readability/braces,+readability/casting,+readability/inheritance,+runtime/casting,+runtime/explicit,+whitespace/blank_line,+whitespace/braces,+whitespace/comma,+whitespace/comments,+whitespace/empty_conditional_body,+whitespace/empty_loop_body,+whitespace/end_of_line,+whitespace/ending_newline,+whitespace/forcolon,+whitespace/indent,+whitespace/line_length,+whitespace/newline,+whitespace/operators,+whitespace/parens,+whitespace/semicolon,+whitespace/tab
        --linelength=100
        --enable=warning,performance,portability,information,missingInclude
        --std=c++11
        --library=qt.cfg
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
        --quiet
        ${ALL_SOURCE_FILES}
)
