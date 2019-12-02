#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

//function_prototypes
void initialize_reg_num_and_name(std::unordered_map< std::string, int > &, std::unordered_map< int, std::string > & );
void print_registers(int [], std::unordered_map< int, std::string > & );
void print_tokens(std::vector< std::vector< std::string > > &);
void input_filename_and_tokenize(std::vector< std::vector< std::string > > &);
void init_labels(std::unordered_map< std::string, int > &, std::vector< std::vector< std::string > > &);
void run_the_program(std::vector< std::vector< std::string > > &, std::unordered_map< std::string, int >  &, std::unordered_map< std::string, int > &, int []);

int main() 
{
    int registers[32] = {0};
    std::vector< std::vector< std::string > > tokens;
    std::unordered_map< std::string, int > labels;          // maps labels to PC
    std::unordered_map< std::string, int > reg_name_to_num; // maps registers to integer
    std::unordered_map< int ,std::string > reg_num_to_name; // maps ints to registers name
    initialize_reg_num_and_name(reg_name_to_num, reg_num_to_name);
    input_filename_and_tokenize(tokens);
    init_labels(labels, tokens);
    print_tokens(tokens);
    print_registers(registers, reg_num_to_name);
    run_the_program(tokens, labels, reg_name_to_num, registers);
    return 0;
}

void run_the_program(std::vector< std::vector< std::string > > & tokens, std::unordered_map< std::string, int >  & labels,
                     std::unordered_map< std::string, int > & reg_name_to_num, int registers[])
{
    if (labels.find("main") == labels.end()) return;  //main not found
    int PC = labels["main"];
    while (PC != -1)
    {
        PC = -1;
    }
    std::cout << PC << std::endl;
    return;
}

void input_filename_and_tokenize(std::vector< std::vector< std::string > > & tokens)
{
    std::string filename;
    std::cin >> filename;
    std::ifstream fp;
    fp.open(filename);
    while (fp)
    {
        std::vector< std::string > current_line_token;
        std::string line_input;
        std::string current_string;
        std::getline(fp, line_input);
        for (int i = 0; i < line_input.size(); ++i)
        {
            if (line_input[i] == ' ' && current_string.size() != 0)
            {
                current_line_token.push_back(current_string);
                current_string.resize(0);
            }
            else if (line_input[i] != ' ') current_string.push_back(line_input[i]);
        }
        if (current_string.size() != 0) current_line_token.push_back(current_string);
        if (current_line_token.size() != 0) tokens.push_back(current_line_token);
    }
    fp.close();
    return;
}

void initialize_reg_num_and_name(std::unordered_map< std::string, int > & reg_name_to_num,
                                 std::unordered_map< int, std::string > & reg_num_to_name)
{
    std::vector< std::string > r_name =
        {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
         "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
         "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
         "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

    for (int i = 0; i < 32; ++i)
    {
        reg_name_to_num[r_name[i]] = i;
        reg_num_to_name[i] = r_name[i];
    }
    return;
}

void print_registers(int registers[], std::unordered_map< int, std::string > & reg_num_to_name)
{
    for (int i = 0; i < 32; ++i)
    {
        std::cout << reg_num_to_name[i] << " = " << registers[i] << "    ";
        if (i % 2 == 1) std::cout << std::endl; 
    }
    return;
}

void print_tokens(std::vector< std::vector< std::string > > & tokens)
{
    for (int i = 0; i < tokens.size(); ++i)
    {
        for (int j = 0; j < tokens[i].size(); ++j)
        {
            std::cout << tokens[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    return;
}

void init_labels(std::unordered_map< std::string, int > & labels, std::vector< std::vector< std::string > > & tokens)
{
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::string first_token = tokens[i][0];
        if (first_token[first_token.size() - 1] == ':')
        {
            labels[std::string(first_token, 0, first_token.size() - 1)] = i;
        }
    }
        return;
}
