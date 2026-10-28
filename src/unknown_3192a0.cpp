// @flags /O2 /GL-
// A one-byte query result returned through MSVC's hidden result pointer.
struct s_query_result_3192a0
{
	bool value;
	s_query_result_3192a0(bool v) : value(v) {}
};
class c_query_3192a0
{
public:
	s_query_result_3192a0 query(void const *) const;
};
// @retail 0x3192a0
s_query_result_3192a0 c_query_3192a0::query(void const *) const
{
	return false;
}
