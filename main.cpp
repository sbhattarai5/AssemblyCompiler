#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

//function_prototypes
void initialize_reg_num_and_name(std::unordered_map< std::string, int > &,
                                 std::unordered_map< int, std::string > & );
void print_registers(int [], std::unordered_map< int, std::string > & );

int main()
{
    int PC;
    std::unordered_map< std::string, int > labels; // maps labels to PC
    std::unordered_map< std::string, int > reg_name_to_num; // maps registers to integer
    std::unordered_map< int ,std::string > reg_num_to_name; // maps ints to registers name
    int registers[32] = {0};
    initialize_reg_num_and_name(reg_name_to_num, reg_num_to_name);
    print_registers(registers, reg_num_to_name);
    return 0;
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
