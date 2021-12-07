from ply import lex
import ply.yacc as yacc

tokens = (
    'NUM',
    'SUB',
    'ADD',
    'MUL',
    'DIV',
    'MOD',
    'POW',
    'LBR',
    'RBR',
)

t_ignore = ' \t'

t_ADD   = r'\+'
t_SUB   = r'\-'
t_MUL   = r'\*'
t_DIV   = r'/'
t_MOD   = r'\%'
t_POW   = r'\^'
t_LBR   = r'\('
t_RBR   = r'\)'
size = 1234577

def t_NUM( t ) :
    r'[0-9]+'
    t.value = int( t.value )
    return t

def t_END( t ):
    r'\n+'
    t.lexer.lineno += len( t.value )

def t_error( t ):
    print("Invalid Token:",t.value[0])
    t.lexer.skip( 1 )

lexer = lex.lex()

precedence = (
    ( 'left', 'ADD', 'SUB' ),
    ( 'left', 'MUL', 'DIV' ),
    ( 'left', 'MOD', 'POW' ),
    ( 'left', 'LBR', 'RBR' ),
    ( 'nonassoc', 'NEG' )
)

def normalize( p ) :
    if p >= size:
        return (p%size)
    i = p
    while i < 0:
        i += size
    return i

def normalizePower( p ) :
    newSize = size - 1
    power = p
    if power >= newSize:
        power = power%newSize
    while power < 0:
        power += newSize

    if p >= newSize:
        return (p%newSize)
    i = p
    while i < 0:
        i += newSize
    return i


def p_add( p ) :
    'expr : expr ADD expr'
    p[0] = normalize(p[1] + p[3])
    print("+ ", end='')
def p_sub( p ) :
    'expr : expr SUB expr'
    p[0] = normalize(p[1] - p[3])
    print("- ", end='')
def p_mul( p ) :
    'expr : expr MUL expr'
    p[0] = normalize(p[1] * p[3])
    print("* ", end='')
def p_div( p ) :
    'expr : expr DIV expr'
    print("/ ", end='')
    for i in range(size-2):
        if normalize(i * p[3]) == p[1]:
            p[0] = i
def p_mod( p ) :
    'expr : expr MOD expr'
    p[0] = normalize(p[1] % p[3])
    print("% ", end='')
def p_pow( p ) :
    'expr : expr POW expp'
    power = p[3]
    newSize = size - 1
    if power >= newSize:
        power = power%newSize
    while power < 0:
        power += newSize
    p[0] = normalize(pow(p[1], power))
    print("^ ", end='')
def p_br( p ) :
    'expr : LBR expr RBR'
    p[0] = p[2]
def p_expr2NUM( p ) :
    'expr : NUM'
    p[0] = normalize(p[1])
    print(p[0], end=' ')
def p_expr2NEG( p ) :
    'expr : SUB expn %prec NEG'
    p[0] = normalize(-p[2])
    print(p[0], end=' ')
def p_expr2NEG_NUM( p ) :
    'expn : NUM'
    p[0] = normalize(p[1])
def p_expr2NEG_BR( p ) :
    'expn : LBR expr RBR'
    p[0] = p[2]

def p_add_power( p ) :
    'expp : expp ADD expp'
    p[0] = normalizePower(p[1] + p[3])
    print("+ ", end='')
def p_sub_power( p ) :
    'expp : expp SUB expp'
    p[0] = normalizePower(p[1] - p[3])
    print("- ", end='')
def p_mul_power( p ) :
    'expp : expp MUL expp'
    p[0] = normalizePower(p[1] * p[3])
    print("* ", end='')
def p_div_power( p ) :
    'expp : expp DIV expp'
    print("/ ", end='')
    for i in range(size-2):
        if normalize(i * p[3]) == p[1]:
            p[0] = i
def p_mod_power( p ) :
    'expp : expp MOD expp'
    p[0] = normalizePower(p[1] % p[3])
    print("% ", end='')
def p_br_power( p ) :
    'expp : LBR expp RBR'
    p[0] = p[2]
def p_expr2NEG_power( p ) :
    'expp : SUB exppn %prec NEG'
    p[0] = -p[2]
    print(p[0], end=' ')
def p_expr2NUM_power_neg( p ) :
    'exppn : NUM'
    p[0] = p[1]
def p_expr2NUM_power( p ) :
    'expp : NUM'
    p[0] = p[1]
    print(normalizePower(p[1]), end=' ')


def p_error( p ):
    print("Syntax error in input!")
    print(p)

parser = yacc.yacc()

while 1:
    res = parser.parse(input())
    print()
    print(' = ', end='')
    print(res)