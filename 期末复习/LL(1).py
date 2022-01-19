def eliminate_left_recursion():
    seq = list(grammar.keys())
    for i in range(len(seq)):
        cur = seq[i]
        for rep in seq[:i]:
            ok = list(filter(lambda x: x[0] != rep, grammar[cur]))
            for right in set(grammar[cur]) - set(ok):
                ok.extend([pre + right[1:] for pre in grammar[rep]])
            grammar[cur] = ok
        targets = set(filter(lambda x: x[0] == cur, grammar[cur]))
        if targets:
            new_left = cur + "'"
            grammar[cur] = [pre + new_left for pre in set(grammar[cur]) - targets]
            grammar[new_left] = [target[1:] + new_left for target in targets] + ['ε']


def first_helper(right: str):
    ret = set()
    for ch in right:
        if str.islower(ch) or ch == 'ε':
            return ret | {ch}
        ret |= first(ch)
        if 'ε' not in ret:
            return ret
        ret -= {'ε'}
    return ret | {'ε'}


def first(non_term: str):
    ret = set()
    for right in grammar[non_term]:
        ret |= first_helper(right)
    return ret


def follow():
    for non_term in grammar:
        follow_set[non_term] = set()
    follow_set['S'] = set('$')
    change = True
    while change:
        change = False
        for left, rights in grammar.items():
            for right in rights:
                for i in range(len(right)):
                    ch = right[i]
                    if str.isupper(ch):
                        size = len(follow_set[ch])
                        follow_set[ch] |= first_helper(right[i + 1:])
                        if 'ε' in follow_set[ch]:
                            follow_set[ch] -= set('ε')
                            follow_set[ch] |= follow_set[left]
                        if size != len(follow_set[ch]):
                            change = True


def construct_ll1_table():
    for left, rights in grammar.items():
        ll1_table[left] = {}
        for right in rights:
            select = first_helper(right)
            if 'ε' in select:
                select -= {'ε'}
                select |= follow_set[left]
            for ch in select:
                if ch in ll1_table[left]:
                    raise Exception("非LL(1)文法")
                ll1_table[left][ch] = right


def analyse(sentence: str):
    sentence = list('$' + sentence[::-1])
    stack = ['$', 'S']
    while stack[-1] != '$':
        left = stack[-1]
        right = sentence[-1]
        if str.islower(left):
            if left != right:
                return False
            stack.pop()
            sentence.pop()
        else:
            if right not in ll1_table[left]:
                return False
            replacer = ll1_table[left][right][::-1]
            stack.pop()
            if replacer != 'ε':
                stack.extend(list(replacer))
    return sentence[-1] == '$'


# 文法规则的存储，从非终结符到产生式右部的映射，默认开始符号为S
grammar: dict[str, list[str]] = {
    'S': ['AB'],
    'A': ['a', 'ε'],
    'B': ['b', 'ε']
}

# follow集的存储，从非终结符到对应的follow集合的映射
follow_set: dict[str, set[str]] = {}

# LL(1)分析表的存储，非终结符遇到终结符时使用的产生式的映射
ll1_table: dict[str, dict[str, str]] = {}

# eliminate_left_recursion()
follow()
construct_ll1_table()
print(ll1_table)
