#include "online_tasks.h"
#include <xtl.h>

/* Native offline compatibility for the recovered task-screen core.  The
 * selected UI path still owns and looks up its retail-format task records,
 * while Xbox Live task handles cannot exist in the native port yet. */

// @retail 0x6b910
s_type_9df9da *function_6b910(long task_index)
{
    return online_task_try_get(task_index);
}

// @retail 0x6c670
HRESULT online_task_continue(s_type_9df9da *task)
{
    if (!task || !task->handle || task->handle == (void *)NONE)
        return E_FAIL;
    return (HRESULT)0x80151000L;
}
