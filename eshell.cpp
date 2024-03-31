#include "eshell.h"

namespace eshell {

auto run(const parsed_input &input, bool repeater) -> void {
    std::vector<single_input> inputs(input.inputs,
                                     input.inputs + input.num_inputs);
    switch (input.separator) {
    case SEPARATOR_NONE:
    case SEPARATOR_SEQ:
        for (const single_input &in : inputs) {
            execute::wait_all(
                execute::execute_pipeline(execute::to_stages(in)));
        }
        break;
    case SEPARATOR_PIPE:
        execute::wait_all(execute::execute_pipeline(inputs));
        break;
    case SEPARATOR_PARA:
        execute::execute_parallel(inputs, repeater);
        break;
    }
}
} // namespace eshell
