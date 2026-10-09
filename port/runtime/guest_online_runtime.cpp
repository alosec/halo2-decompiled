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

// @retail 0x6b640
void function_6b640(long task_index)
{
    s_type_9df9da *task = online_task_try_get(task_index);
    if (task)
    {
        /* Native task handles cannot own Xbox Live resources. */
        task->handle = 0;
        record_pool_release(g_4cf78c, task_index);
    }
}
