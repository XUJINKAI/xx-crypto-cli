#include "cmdparser.h"
#include "global.h"

extern cmdp_command_st sc_echo;
extern cmdp_command_st sc_gen;
extern cmdp_command_st sc_hex;
extern cmdp_command_st sc_rand;
extern cmdp_command_st sc_sm4;
static cmdp_command_st main_cmdp = {
    .sub_commands =
        (cmdp_command_st *[]){
            &sc_echo,
            &sc_gen,
            &sc_hex,
            &sc_rand,
            &sc_sm4,
            NULL,
        },
};

int main(int argc, char *argv[])
{
    return cmdp_run(argc - 1, argv + 1, &main_cmdp);
}
