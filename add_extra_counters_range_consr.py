import os
import re
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(description='extra_counter_adder',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter
                                     )
    parser.add_argument('-dir', '--directory_path', required=True)
    parser.add_argument('mode', default='add', choices=['add', 'remove'])
    return parser.parse_args()

def count_occurrences(file_path, pattern):
    count = 0
    with open(file_path, 'r') as file:
        for line in file:
            if pattern in line:
                count += 1
    return count

def insert_code(file_path, code, pattern):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    with open(file_path, 'w') as file:
        for line in lines:
            file.write(line)
            if pattern in line:
                file.write(code)

def insert_code_with_index(file_path, code, pattern):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    with open(file_path, 'w') as file:
        index = 0
        for line in lines:
            file.write(line)
            if pattern in line:
                file.write(code.format(index))
                index += 1

def remove_inserted_code(file_path, start_pattern, end_pattern):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    with open(file_path, 'w') as file:
        insert_mode = False
        for line in lines:
            if start_pattern in line:
                insert_mode = True
            if not insert_mode:
                file.write(line)
            if end_pattern in line:
                insert_mode = False

def process_circom_file(directory):
    for file_name in os.listdir(directory):
        if file_name.endswith(".circom"):
            name = file_name.split(".circom")[0]
            cpp_file_path = os.path.join(directory, f"{name}_cpp/{name}.cpp")
            
            count = count_occurrences(cpp_file_path, "Fr_copy(aux_dest,")
            code_to_insert = """
// generate extra counters start
#include <cstring>
#include <gmpxx.h>
static const size_t ELEMENTS_COUNT = {};
static const size_t CHECKS_COUNT = 3;

#ifdef __linux__
__attribute__((section("__libfuzzer_extra_counters")))
#endif
static uint8_t extra_counters[ELEMENTS_COUNT + ELEMENTS_COUNT * 253 + 1];

static uint8_t is_not_bool_elements[ELEMENTS_COUNT];
static size_t proccess_count = 0;

void proccess_bool_element(PFrElement signal_element, size_t index){{
    char* str_element = Fr_element2str(signal_element);
    if (strlen(str_element) != 1 || (str_element[0] != '0' && str_element[0] != '1')){{
        is_not_bool_elements[index] = 1;
    }}
    if (index == ELEMENTS_COUNT - 1) ++proccess_count;
    if (proccess_count < CHECKS_COUNT || is_not_bool_elements[index]){{
        free(str_element);
        return;
    }}
    extra_counters[index] = (str_element[0] == '1');
    free(str_element);
}}

void proccess_overflow_element(PFrElement signal_element, size_t index){{
    char* str_element = Fr_element2str(signal_element);
    mpz_class element;
    element = str_element;
    int highest_bit = mpz_sizeinbase(element.get_mpz_t(), 2) - 1;
    extra_counters[ELEMENTS_COUNT + index*253 + highest_bit] = 1;
    free(str_element);
}}
// generate extra counters end
""".format(count)

            func_to_insert_bool = """
// generate extra counters start
proccess_bool_element(aux_dest, {});
// generate extra counters end
"""
            func_to_insert_overflow = """
// generate extra counters start
proccess_overflow_element(aux_dest, {});
// generate extra counters end
"""

            insert_code(cpp_file_path, code_to_insert, "// function declarations")
            insert_code_with_index(cpp_file_path, func_to_insert_bool, "Fr_copy(aux_dest,")
            insert_code_with_index(cpp_file_path, func_to_insert_overflow, "Fr_copy(aux_dest,")


if __name__ == '__main__':
    args = parse_arguments()
    directory_path = args.directory_path
    match args.mode:
        case 'add':
            process_circom_file(directory_path)
        case 'remove':
            for file_name in os.listdir(directory_path):
                if file_name.endswith(".circom"):
                    name = file_name.split(".circom")[0]
                    cpp_file_path = os.path.join(directory_path, f"{name}_cpp/{name}.cpp")
                    remove_inserted_code(cpp_file_path, "// generate extra counters start", "// generate extra counters end")