from drafts import disabled_addresses


def write(tmp_path, name, text):
    path = tmp_path / name
    path.write_text(text)
    return path


def test_marker_inside_if_zero_is_disabled(tmp_path):
    write(tmp_path, 'a.cpp', '''// @retail 0x100
void a(void) {}

#if 0
// Retail 0x200, disabled with its blocker above.
void b(void) {}
#endif

// @retail 0x300
void c(void) {}
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x200}


def test_nested_branches_and_the_enabled_else_branch(tmp_path):
    write(tmp_path, 'b.cpp', '''#if 0
// @retail 0x10
#if 1
// Retail 0x20
#endif
#else
// @retail 0x30
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x10, 0x20}


def test_elif_zero_is_disabled_and_other_elif_is_not(tmp_path):
    write(tmp_path, 'c.cpp', '''#if 1
// @retail 0x40
#elif 0
// Retail 0x50
#else
// @retail 0x60
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x50}


def test_marker_is_case_insensitive_and_files_are_recursive(tmp_path):
    (tmp_path / 'sub').mkdir()
    write(tmp_path / 'sub', 'd.cpp', '#if 0\n// retail 0xABCD\n#endif\n')
    write(tmp_path, 'e.h', '#if 0\n// Retail 0x1\n#endif\n')
    assert set(disabled_addresses(str(tmp_path))) == {0xabcd}


def test_conditional_takes_its_trailing_comment_off(tmp_path):
    write(tmp_path, 'f.cpp', '#if 0 // Activation changes matched helpers.\n// Retail 0x70\n#endif\n')
    assert set(disabled_addresses(str(tmp_path))) == {0x70}


def test_disabled_implementation_comment_names_the_function(tmp_path):
    write(tmp_path, 'g.cpp', '''#if 0
// Disabled implementation for 0x6d4c0.
void function_6d4c0(void *object)
{
}
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x6d4c0}


def test_function_definition_without_a_marker_is_a_draft(tmp_path):
    write(tmp_path, 'h.cpp', '''#if 0
void function_06d4c0(void *object)
{
}
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x6d4c0}


def test_a_multiline_signature_is_a_draft(tmp_path):
    write(tmp_path, 'i.cpp', '''#if 0
static void function_6d4c0(
    void *object)
{
}
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == {0x6d4c0}


def test_a_call_inside_if_zero_is_not_a_draft(tmp_path):
    write(tmp_path, 'j.cpp', '''#if 0
void helper(void)
{
    function_6d4c0(0);
}
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == set()


def test_a_prototype_inside_if_zero_is_not_a_draft(tmp_path):
    write(tmp_path, 'k.cpp', '''#if 0
void function_6d4c0(void *object);
#endif
''')
    assert set(disabled_addresses(str(tmp_path))) == set()
