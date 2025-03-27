#include "engine/config.h"

#include "engine/rdma_transport.h"
#include "ops/ops.h"
#include "utils/json.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using json = nlohmann::json;

namespace py = pybind11;

PYBIND11_MODULE(_slime_c, m)
{
    py::class_<slime::RDMAContext>(m, "rdma_context")
        .def(py::init<>())
        .def("init_rdma_context", &slime::RDMAContext::init_rdma_context)
        .def("register_memory", &slime::RDMAContext::register_memory)
        .def("exchange_info", [](slime::RDMAContext& self) { return self.exchange_info().dump(); })
        .def("connect",
             [](slime::RDMAContext& self, std::string exchange_info) {
                 json            json_info       = json::parse(exchange_info);
                 slime::RDMAInfo local_rdma_info = slime::RDMAInfo(json_info["rdma_info"]);
                 self.modify_qp_to_rtsr(local_rdma_info);
                 for (auto& mr_info : json_info["mr_info"].items())
                     self.register_remote_memory(mr_info.key(), mr_info.value());
             })
        .def("cq_poll_handle", &slime::RDMAContext::cq_poll_handle)
        .def("launch_cq_future", &slime::RDMAContext::launch_cq_future)
        .def("stop_cq_future", &slime::RDMAContext::stop_cq_future)
        .def("r_rdma_async",
             &slime::RDMAContext::r_rdma_async,
             py::call_guard<py::gil_scoped_release>(),
             "Read remote memory asynchronously")
        .def("batch_r_rdma_async",
             &slime::RDMAContext::batch_r_rdma_async,
             py::call_guard<py::gil_scoped_release>(),
             "Read remote memory asynchronously");

    m.def("gather", &slime::gather);
    m.def("scatter", &slime::scatter);
}
