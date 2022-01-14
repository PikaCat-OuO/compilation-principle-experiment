class TreeNode:
    def __init__(self):
        self.child: list = [0, 0]
        self.data: str = ''


def match(token):
    global TOKEN, INDEX
    if TOKEN == token:
        INDEX = INDEX + 1
        TOKEN = re[INDEX]
    else:
        raise Exception("未匹配的符号" + TOKEN)


def exp():
    node1 = term()
    while TOKEN == '+' or TOKEN == '-':
        node = TreeNode()
        node.data = TOKEN
        match(TOKEN)
        node.child[0] = node1
        node.child[1] = term()
        node1 = node
    return node1


def term():
    node1 = factor()
    while TOKEN == '*' or TOKEN == '/':
        node = TreeNode()
        node.data = TOKEN
        match(TOKEN)
        node.child[0] = node1
        node.child[1] = factor()
        node1 = node
    return node1


def factor():
    node = TreeNode()
    node.data = TOKEN
    match(TOKEN)
    return node


def print_tree(node, depth):
    print(depth * ' ' + node.data)
    if node.child[0]:
        print_tree(node.child[0], depth + 1)
    if node.child[1]:
        print_tree(node.child[1], depth + 1)


INDEX = 0
re = 'z+d+b*a+b+c/d$'
TOKEN = re[0]
print_tree(exp(), 0)
