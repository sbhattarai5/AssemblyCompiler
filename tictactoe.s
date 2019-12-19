    .text  
    .globl main


print_string:
    li $v0, 4
    syscall
    jr $ra

read_int:
    li $v0, 5
    syscall
    jr $ra


################################################
# Initializing the board to all zero
################################################
initialize_board:
    la $t0, board                   ## loading the address of the board
    mul $a0, $a0, $a0               ## n * n board  
    addu $a0, $a0, $a0                     
    addu $a0, $a0, $a0
    addu $a0, $t0, $a0              ## a0 = &board[n * n]
LOOP1:

    bge $t0, $a0, ENDLOOP1           ## if (t0 >= a0) goto ENDLOOP   
    sw $zero, 0($t0)                ## setting to zero
    addiu $t0, $t0, 4                    ## incrementing current index by 1
    j LOOP1

ENDLOOP1:
    
    move $t0, $s0        # t0 = n
    li $t1, 2            # t1 = 2 
    addi $t0, $t0, -1         # t0 = n - 1       
    div $t0, $t1         # t0 / 2
    mflo $t0             # t0 = (n - 1) / 2
    move $t1, $t0        # t1 = (n - 1) / 2   
    mul $t0, $t0, $s0    # t0 = (n - 1) / 2 * n    
    add $t0, $t0, $t1    # t0 = (n - 1) / 2 * n + (n - 1) / 2  
    add $t0, $t0, $t0    
    add $t0, $t0, $t0    # t0 = 4 * t0
    la $t1, board        #    
    add $t0, $t1, $t0
    li $t1, 1
    sw $t1, 0($t0)       #check here
    jr $ra

#################################################


#################################################
## print_board
## +-+-+-+
## | | | |
## +-+-+-+
## | | | |
## +-+-+-+
## | | | |
## +-+-+-+ 
#################################################
print_board:
    li $t0, 0       # t0 = row index
    la $t1, board   # t1 = current address
    sw $ra, 0($sp)
    jal print_plusminusbar
    
LOOP2:

    bge $t0, $s0, ENDLOOP2
    li $t2, 0        # t2 = col index
    la $a0, PIPE      
    li $v0, 4
    syscall

LOOP3:

    bge $t2, $s0, ENDLOOP3
    lw $a0, 0($t1)
    add $a0, $a0, $a0
    add $a0, $a0, $a0   # a0 = 4 * a0
    la $t3, L
    addu $a0, $t3, $a0
    lw $a0, 0($a0)
    jr $a0

L0:
    la $a0, SPACE
    j ENDCASE
L1:
    la $a0, my_piece
    j ENDCASE
L2:
    la $a0, opp_piece
    j ENDCASE
ENDCASE:
    
    li $v0, 4
    syscall
    la $a0, PIPE
    li $v0, 4
    syscall
    addiu $t1, $t1, 4 
    addi $t2, $t2, 1
    j LOOP3

ENDLOOP3:    
    
    la $a0, NEWLINE
    li $v0, 4
    syscall
    jal print_plusminusbar   
    addi $t0, $t0, 1    
    j LOOP2

ENDLOOP2:

    lw $ra, 0($sp)
    jr $ra
################################################

###############################################
## place piece on board
###############################################
place_piece_on_board:
    mul $a0, $a0, $s0     # a0 = row * n
    add $a0, $a0, $a1     # a0 = row * n + col
    add $a0, $a0, $a0     
    add $a0, $a0, $a0     # a0 = 4 * a0 
    la $t0, board
    add $t0, $t0, $a0
    la $t1, turn 
    lw $t1, 0($t1)
    sw $t1, 0($t0)
    li $t0, 1
    beq $t0, $t1, make_X_turn
    la $t1, turn
    sw $t0, 0($t1)    # make_O_turn
    j ENDIF

make_X_turn:

    li $t0, 2
    la $t1, turn
    sw $t0, 0($t1)  

ENDIF:
    jr $ra


################################################
## check_col 
################################################
check_col:
    li $t0, 0
LOOP6:
    beq $t0, $s0, ENDLOOP6
    la $t1, board
    move $t6, $t0
    add $t6, $t6, $t6
    add $t6, $t6, $t6
    add $t1, $t1, $t6
    lw $t2, 0($t1)
    li $t4, -1
    beq $t2, $zero, ENDLOOP7
    mul $t3, $s0, $s0     # t3 = n *  n
    add $t3, $t3, $t3
    add $t3, $t3, $t3     # t3 = 4 * n * n
    la $t6, board
    add $t3, $t3, $t6     # t3 = base address of board + 4 * n * n
    LOOP7:
        bge $t1, $t3, ENDLOOP7
        lw $t4, 0($t1)
        bne $t4, $t2, ENDLOOP7
        add $t6, $s0, $s0
        add $t6, $t6, $t6
        add $t1, $t1, $t6   # jump to next row
        j LOOP7
    ENDLOOP7:   
    beq $t4, $t2, WINNER_FOUND
    j ENDIF3
    WINNER_FOUND:
        move $s1, $t4
        j ENDLOOP6
    ENDIF3:
    addi $t0, $t0, 1
    j LOOP6
ENDLOOP6:
    jr $ra
################################################

################################################
## check_row
################################################
check_row:
    li $t0, 0
LOOP8:
    beq $t0, $s0, ENDLOOP8
    la $t1, board
    mul $t6, $t0, $s0
    add $t6, $t6, $t6
    add $t6, $t6, $t6
    add $t1, $t1, $t6
    lw $t2, 0($t1)
    li $t4, -1
    beq $t2, $zero, ENDLOOP9
    li $t7, 4
    mul $t3, $s0, $t7       # t3 = 4 *  n
    add $t3, $t1, $t3     # t3 = starting address + 4 * n
    LOOP9:
        bge $t1, $t3, ENDLOOP9
        lw $t4, 0($t1)
        bne $t4, $t2, ENDLOOP9
        addi $t1, $t1, 4
        j LOOP9
    ENDLOOP9:   
    beq $t4, $t2, WINNER_FOUND2
    j ENDIF5
    WINNER_FOUND2:
        move $s1, $t4
        j ENDLOOP8
    ENDIF5:
    addi $t0, $t0, 1
    j LOOP8
ENDLOOP8:
    jr $ra
################################################

################################################
## check first diagonal
###############################################
check_first_diag:
    li $t0, 0
    la $t1, board
    lw $t2, 0($t1)
    li $t4, -1
    beq $t2, $zero, ENDLOOP10
    LOOP10:
        bge $t0, $s0, ENDLOOP10
        lw $t4, 0($t1)
        bne $t4, $t2, ENDLOOP10
        add $t3, $s0, $s0
        add $t3, $t3, $t3
        addi $t3, $t3, 4
        add $t1, $t1, $t3
        addi $t0, $t0, 1
        j LOOP10
    ENDLOOP10:   
    beq $t4, $t2, WINNER_FOUND3
    j ENDIF4
    WINNER_FOUND3:
        move $s1, $t4
    ENDIF4:
    jr $ra


################################################
## check second diagonal
###############################################
check_second_diag:
    li $t0, 0
    la $t1, board
    add $t2, $s0, $s0
    add $t2, $t2, $t2
    addi $t2, $t2, -4
    add $t1, $t1, $t2
    lw $t2, 0($t1)
    li $t4, -1
    beq $t2, $zero, ENDLOOP11
    LOOP11:
        bge $t0, $s0, ENDLOOP11
        lw $t4, 0($t1)
        bne $t4, $t2, ENDLOOP11
        add $t5, $s0, $s0
        add $t5, $t5, $t5
        addi $t5, $t5, -4
        add $t1, $t1, $t5
        addi $t0, $t0, 1
        j LOOP11
    ENDLOOP11:   
    beq $t4, $t2, WINNER_FOUND4
    j ENDIF6
    WINNER_FOUND4:
        move $s1, $t4
    ENDIF6:
    jr $ra


################################################
##  check_draw
################################################
check_draw:
    la $t0, board
    li $t1, 0
    mul $t2, $s0, $s0
        
LOOP12:

        
    beq $t1, $t2, ENDLOOP12
    lw $t3, 0($t0)
    bne $t3, $zero, ENDIF7    
    jr $ra

ENDIF7:

    addi $t0, $t0, 4
    addi $t1, $t1, 1
    j LOOP12

ENDLOOP12:

    li $s1, 0
    jr $ra

################################################
## check win function
################################################
check_winner:
    sw $ra, -4($sp)
    li $t5, -1
    jal check_row
    bne $s1, $t5, ENDIF2
    jal check_col
    bne $s1, $t5, ENDIF2
    jal check_first_diag
    bne $s1, $t5, ENDIF2
    jal check_second_diag
    li $t5, -1
    bne $s1, $t5, ENDIF2
    jal check_draw
        
ENDIF2:    
    lw $ra, -4($sp)
    jr $ra

################################################
## generate_move
## finds the move and set $a0, and $a1 to row and col, $a0 has a value in it which indicates the turn
################################################
generate_move:
    move $s2, $ra
 
 ##look for row winner
    li $t0, 0
    la $t1, board
    mul $t3, $s0, $s0
LOOP13:
    beq $t0, $t3, ENDLOOP13
    lw $t2, 0($t1)
    bne $t2, $zero, ENDIF8
    sw $a0, 0($t1)
    move $s4, $t0
    move $s5, $t1
    move $s6, $t2
    move $s7, $t3
    jal check_row
    move $t0, $s4
    move $t1, $s5
    move $t2, $s6
    move $t3, $s7
    beq $s1, $a0, WIN_MOVE_FOUND
    sw $zero, 0($t1)
    j ENDIF9
    WIN_MOVE_FOUND:
        sw $zero, 0($t1)
        div $t0, $s0
        mflo $a0
        mfhi $a1
        move $ra, $s2
        jr $ra
    ENDIF9:
    ENDIF8:
    addi $t1, $t1, 4
    addi $t0, $t0, 1
    j LOOP13
ENDLOOP13:

##LOOK FOR COLUMN WINNER

li $t0, 0
LOOP14:
    beq $t0, $s0, ENDLOOP14
    li $t1, 0
    la $t2, board
    add $t3, $t0, $t0
    add $t3, $t3, $t3
    add $t2, $t2, $t3
    LOOP15:
        beq $t1, $s0, ENDLOOP15
        lw $t4, 0($t2)
        bne $t4, $zero, ENDIF10
        sw $a0, 0($t2)
        move $s4, $t0
        move $s5, $t1
        move $s6, $t2
        move $s7, $t3
        jal check_col
        move $t0, $s4
        move $t1, $s5
        move $t2, $s6
        move $t3, $s7
        beq $s1, $a0, WIN_MOVE_FOUND2
        sw $zero, 0($t2)
        j ENDIF11
        WIN_MOVE_FOUND2:
            sw $zero, 0($t2)
            move $a0, $t1
            move $a1, $t0
            jr $s2
        
        ENDIF11: 
        ENDIF10:
        
        addi $t1, $t1, 1
        add $t4, $s0, $s0
        add $t4, $t4, $t4
        add $t2, $t2, $t4
        j LOOP15
    
    ENDLOOP15:
    addi $t0, $t0, 1
    j LOOP14
ENDLOOP14:

##look for first diagonal winner

li $t0, 0
la $t1, board
LOOP17:
    beq $t0, $s0, ENDLOOP17
    lw $t2, 0($t1)
    bne $t2, $zero, ENDIF13
    sw $a0, 0($t1)
    move $s3, $t0
    move $s4, $t1
    jal check_first_diag
    move $t0, $s3
    move $t1, $s4
    sw $zero, 0($t1)
    beq $s1, $a0, WIN_MOVE_FOUND3
    j ENDIF14
    WIN_MOVE_FOUND3:
        move $a0, $t0
        move $a1, $a0
        jr $s2
    ENDIF14:
    ENDIF13:
    add $t3, $s0, $s0
    add $t3, $t3, $t3
    add $t1, $t1, $t3
    addi $t1, $t1, 4
    addi $t0, $t0, 1
    j LOOP17
ENDLOOP17:

##for second_diagonal_winner

li $t0, 0
la $t1, board
addi $t2, $s0, -1
add $t2, $t2, $t2
add $t2, $t2, $t2
add $t1, $t1, $t2
LOOP18:
    beq $t0, $s0, ENDLOOP18
    lw $t3, 0($t1)
    bne $t3, $zero, ENDIF15
    sw $a0, 0($t1)
    move $s3, $t0
    move $s4, $t1
    move $s5, $t2
    jal check_second_diag
    move $t0, $s3
    move $t1, $s4
    move $t2, $s5
    sw $zero, 0($t1)
    beq $s1, $a0, WIN_MOVE_FOUND4
    j ENDIF16
    WIN_MOVE_FOUND4:
        move $a0, $t0
        addi $a1, $s0, -1
        sub $a1, $a1, $a0
        jr $s2
    ENDIF16:
    ENDIF15:
    addi $t0, $t0, 1
    add $t1, $t1, $t2
    j LOOP18
ENDLOOP18:
    jr $s2


####################################################
## finds the first empty move
####################################################

sequential_move:
    li $t0, 0
    la $t1, board
    mul $t3, $s0, $s0
LOOP16:
    beq $t0, $t3, ENDLOOP16
    lw $t2, 0($t1)
    beq $t2, $zero, MOVE_FOUND 
    j ENDIF12
    MOVE_FOUND:
        div $t0, $s0  
        mflo $a0
        mfhi $a1
        jr $ra                
    ENDIF12:
    addi $t1, $t1, 4
    addi $t0, $t0, 1
    j LOOP16
ENDLOOP16:
    jr $ra


################################################
## print plus minus bar (+-+-+-+-+-+-)
################################################
print_plusminusbar:
    
    li $t3, 0

LOOP4:

    bge $t3, $s0, ENDLOOP4
    la $a0, PLUSMINUS
    li $v0, 4
    syscall
    addi $t3, $t3, 1
    j LOOP4

ENDLOOP4:

    la $a0, PLUS
    li $v0, 4
    syscall
    la $a0, NEWLINE
    li $v0, 4
    syscall
    jr $ra

main:

    la $a0, welcome_msg         # displays the welcome message
    jal print_string            # asks for input  
    la $a0, enter_n
    jal print_string
    jal read_int                    
    move $s0, $v0               # s0 = n
    move $a0, $v0               # a0 = n


    jal initialize_board
    
    la $a0, ill_go_first
    jal print_string
    li $s1, -1          # winner 
    li $s3, 0           # bool our_turn
    jal print_board                 

LOOP5:

    li $t0, -1
    bne $t0, $s1, ENDLOOP5
    la $t0, turn_case
    la $t1, turn
    lw $t1, 0($t1)
    add $t1, $t1, $t1
    add $t1, $t1, $t1
    add $t0, $t0, $t1
    lw $t0, 0($t0)
    jr $t0

blank_case_turn_zero:

our_turn:
    
    li $a0, 1
    jal generate_move
    li $t0, -1
    bne $s1, $t0, ENDIF17
    li $a0, 2
    jal generate_move
    li $t0, -1
    bne $s1, $t0, ENDIF17
    jal sequential_move
    ENDIF17:
    li $s1, -1
    j make_move

user_turn:

    la $a0, enter_row
    jal print_string

    jal read_int
    move $a2, $v0               # a2 = input_row  

    la $a0, enter_col
    jal print_string

    jal read_int               
    move $a1, $v0               # a1 = input_col
    move $a0, $a2               # a0 = input_row    

make_move:
    
    jal place_piece_on_board
    jal print_board
    jal check_winner
    j LOOP5

ENDLOOP5:
  
    add $s1, $s1, $s1
    add $s1, $s1, $s1
    la $t1, RESULT
    add $t1, $t1, $s1
    lw $t1, 0($t1)
    jr $t1

DRAW:
    la $a0, DRAWMSG
    j ENDCASE2

WIN:
    la $a0, WINMSG
    j ENDCASE2

LOSE:
    la $a0, LOSEMSG
    j ENDCASE2

ENDCASE2:
    jal print_string
    li $v0, 10                  # exit()
    syscall
    
##data segment    
    
    .data

welcome_msg: .asciiz "Let's play a game of tic-tac-toe.\n"
enter_n: .asciiz "Enter n: "
ill_go_first: .asciiz "I'll go first.\n"
enter_row: .asciiz "Enter row: "
enter_col: .asciiz "Enter col: "
SPACE: .asciiz " "
opp_piece: .asciiz "X"
my_piece: .asciiz "O"
turn: .word 2
turn_case: .word blank_case_turn_zero our_turn user_turn
NEWLINE: .asciiz "\n"
PLUS: .asciiz "+"
MINUS: .asciiz "-"
PLUSMINUS: .asciiz "+-"
PIPE: .asciiz "|"
DRAWMSG: .asciiz "We have a draw."
WINMSG: .asciiz "I'm the winner!"
LOSEMSG: .asciiz "You are the winner!"
L: .word L0 L1 L2
RESULT: .word DRAW WIN LOSE
board: .word 0
