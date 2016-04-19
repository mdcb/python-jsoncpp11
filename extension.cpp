
#include <iostream>
#include <exception>
#include <pybind11/pybind11.h>
#include <json/json.h>

namespace py = pybind11;

class pyjson : public Json::Value
{

public:

  using Json::Value::Value;

  pyjson()
    : Json::Value(Json::nullValue)
  {}

  pyjson(py::object obj)
    : Json::Value(Json::nullValue)
  {
    fromPyObject(obj);
  }

  std::string toString()
  {
    Json::FastWriter jwriter;
    return jwriter.write(*this);
  }

  void pprint()
  {
    std::cerr << toStyledString();
  }

  py::object toPyObject()
  {
    return pj_convert(*this);
  }

  void fromPyObject(py::object obj)
  {
    PyObject * obj_ptr = obj.ptr();
    jp_convert(obj_ptr, *this);
  }

private:

  void jp_convert(PyObject * obj_ptr, Json::Value & node)
  {

    if (obj_ptr == Py_None)
      {
        node = Json::Value(Json::nullValue);
      }

    else if (PyBytes_Check(obj_ptr))
      {
        // bytes as string
        node = std::string(PyBytes_AsString(obj_ptr));
      }

    else if (PyUnicode_Check(obj_ptr))
      {
        // unicode as ascii string
        node = std::string(PyBytes_AsString(PyUnicode_AsASCIIString(obj_ptr)));
      }

    else if (PySequence_Check(obj_ptr))
      {
        // list have mapping protocol, handle first
        node = Json::Value(Json::ValueType::arrayValue);

        for (Py_ssize_t i = 0; i < PySequence_Length(obj_ptr); i++)
          {
            PyObject * elt = PySequence_GetItem(obj_ptr, i);
            Json::Value elty;
            jp_convert(elt, elty);
            node.append(elty);
          }
      }

    else if (PyMapping_Check(obj_ptr))
      {
        node = Json::Value(Json::ValueType::objectValue);

        PyObject * keyvals = PyMapping_Items(obj_ptr);

        for (Py_ssize_t i = 0; i < PyMapping_Length(keyvals); i++)
          {
            PyObject * kv = PyList_GetItem(keyvals, i);
            PyObject * k = PyTuple_GetItem(kv, 0);
            PyObject * v = PyTuple_GetItem(kv, 1);
            Json::Value * peltk = new Json::Value(); // refcount
            Json::Value eltv;
            jp_convert(k, *peltk);
            jp_convert(v, eltv);
            node[peltk->asString()] = eltv; // only support string keys
          }
      }

    else if (PyBool_Check(obj_ptr))
      {
        node = static_cast<bool>(PyLong_AsLong(obj_ptr));
      }

    else if (PyLong_Check(obj_ptr))
      {
        node = (Json::LargestInt) PyLong_AsLong(obj_ptr); // json cast
      }

    else if (PyFloat_Check(obj_ptr))
      {
        node = PyFloat_AsDouble(obj_ptr);
      }

    else
      {
        throw std::runtime_error("invalid type for conversion.");
      }
  }

  py::object pj_convert(Json::Value const & node)
  {

    switch (node.type())
      {

      case Json::ValueType::nullValue:
      {
        //return py::object();
        return py::object(Py_None, true);
      }

      case Json::ValueType::intValue:
      {
        //node.asInt();
        //node.asInt64();
        return py::int_(node.asLargestInt());
      }

      case Json::ValueType::uintValue:
      {
        //node.asUInt();
        //node.asUInt64();
        return py::int_(node.asLargestUInt());
      }

      case Json::ValueType::realValue:
      {
        //node.asFloat();
        return py::float_(node.asDouble());
      }

      case Json::ValueType::stringValue:
      {
        return py::str(node.asString());
      }

      case Json::ValueType::booleanValue:
      {
        return py::bool_(node.asBool());
      }

      case Json::ValueType::arrayValue:
      {
        py::list result;

        for (auto it = node.begin(); it != node.end(); ++it)
          {
            result.append(pj_convert(*it));
          }

        return result;
      }

      case Json::ValueType::objectValue:
      {
        py::dict result;

        for (auto it = node.begin(); it != node.end(); ++it)
          {
            result[pj_convert(it.key())] = pj_convert(*it);
          }

        return result;
      }

      default:
      {
        throw std::runtime_error("undefined json value");
      }

      }
  }
  
  pyjson(const pyjson &) = delete;
  pyjson & operator= (const pyjson &) = delete;
  pyjson(const pyjson &&) = delete;
  pyjson & operator= (const pyjson &&) = delete;

};

PYBIND11_PLUGIN(jsoncpp)
{
  py::module m("jsoncpp", "pybind11 jsoncpp binding");

  py::class_<pyjson>(m, "json")
  .alias<Json::Value>()
  .def(py::init<>())
  .def(py::init<py::object>())
  .def("pprint", &pyjson::pprint)
  .def_property("data", &pyjson::toPyObject, &pyjson::fromPyObject)
  .def("__str__", &pyjson::toString)
  ;

  // py::implicitly_convertible<Json::Value, pyjson>(); // inputType, outputType

  return m.ptr();
}


