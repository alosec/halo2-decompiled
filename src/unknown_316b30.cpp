// @flags /O2 /GL-
// Havok (third-party physics library): two timers advanced by a step; when
// one passes its period every entry's listener is asked to act, and the
// answer says whether all of them did (the first timer also waits for five
// successful rounds in a row).

struct answer_316b30
{
	char value;

	answer_316b30() {}
	answer_316b30(bool value_) : value((char)value_) {}
};

struct entry_316b30;

class listener_316b30
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual answer_316b30 slot_5(entry_316b30 *entry) { return answer_316b30(); }
	virtual answer_316b30 slot_6(entry_316b30 *entry) { return answer_316b30(); }
};

struct entry_316b30
{
	char unknown0[0x54];
	listener_316b30 *listener;
};

struct settings_316b30
{
	char unknown0[0x30];
	float period_a;
	float period_b;
};

class timers_316b30
{
public:
	char unknown0[0x20];
	settings_316b30 const *settings;
	char unknown24[8];
	char rounds;
	char unknown2d[7];
	float timer_a;
	float timer_b;
	entry_316b30 **entries;
	long count;

	answer_316b30 function_316b30(float const *step);
};

// @retail 0x316b30
answer_316b30 timers_316b30::function_316b30(float const *step)
{
	timer_a += *step;
	float const b = timer_b + *step;
	timer_b = b;
	settings_316b30 const *s = settings;
	bool const a_due = timer_a > s->period_a;
	bool const b_due = b > s->period_b;
	if (a_due)
	{
		timer_a -= s->period_a;
		bool all = true;
		for (long i = 0; i < count; i++)
		{
			if (!entries[i]->listener || !entries[i]->listener->slot_5(entries[i]).value)
			{
				all = false;
			}
		}
		if (!all)
		{
			return false;
		}
		if (rounds < 5)
		{
			rounds++;
			return false;
		}
		rounds = 0;
		return true;
	}
	if (b_due)
	{
		timer_b = b - s->period_b;
		bool all = true;
		for (long i = 0; i < count; i++)
		{
			if (!entries[i]->listener || !entries[i]->listener->slot_6(entries[i]).value)
			{
				all = false;
			}
		}
		if (all)
		{
			return true;
		}
	}
	return false;
}
