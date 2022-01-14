INDEX = 0
s = 'A(B(D,E(G,H)),C(,F))'
TOKEN = s[0]


def match(token):
    global TOKEN, INDEX
    if TOKEN == token:
        INDEX = INDEX + 1
        if INDEX < len(s):
            TOKEN = s[INDEX]
        else:
            TOKEN = '$'
    else:
        raise Exception("未匹配的符号" + TOKEN)


class TreeNode:
    def __init__(self):
        self.left_child = 0
        self.right_child = 0
        self.data = ''


def print_tree(node, depth):
    print(depth * ' ' + node.data)
    if node.left_child:
        print_tree(node.left_child, depth + 1)
    if node.right_child:
        print_tree(node.right_child, depth + 1)


def start():
    node = TreeNode()
    node.data = TOKEN
    match(TOKEN)
    if TOKEN == '(':
        match('(')
        if TOKEN != ',':
            node.left_child = start()
        if TOKEN == ',':
            match(',')
            node.right_child = start()
        match(')')
    return node


print_tree(start(), 0)
