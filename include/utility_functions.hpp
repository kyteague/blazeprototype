//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DYND__UTILITY_FUNCTIONS_HPP_
#define _DYND__UTILITY_FUNCTIONS_HPP_

#include "Python.h"

#include <sstream>
#include <stdexcept>

#include <dynd/dtype.hpp>

namespace pydynd {

/**
 * Function which casts the parameter to
 * a PyObject pointer and calls Py_XDECREF on it.
 */
void py_decref_function(void* obj);

/**
 * A container class for managing the local lifetime of
 * PyObject *.
 *
 * Throws an exception if the object passed into the constructor
 * is NULL.
 */
class pyobject_ownref {
    PyObject *m_obj;

    // Non-copyable
    pyobject_ownref(const pyobject_ownref&);
    pyobject_ownref& operator=(const pyobject_ownref&);
public:
    pyobject_ownref()
        : m_obj(NULL)
    {
    }
    explicit pyobject_ownref(PyObject* obj)
        : m_obj(obj)
    {
        if (obj == NULL) {
            throw std::runtime_error("propagating a Python exception...need a mechanism to do that through Cython with a C++ exception");
        }
    }

    ~pyobject_ownref()
    {
        Py_XDECREF(m_obj);
    }

    PyObject *get()
    {
        return m_obj;
    }

    // Returns a borrowed reference
    operator PyObject *()
    {
        return m_obj;
    }

    /**
     * Returns the reference owned by this object,
     * use it like "return obj.release()". After the
     * call, this object contains NULL.
     */
    PyObject *release()
    {
        PyObject *result = m_obj;
        m_obj = NULL;
        return result;
    }
};

intptr_t pyobject_as_index(PyObject *index);
int pyobject_as_int_index(PyObject *index);
dynd::irange pyobject_as_irange(PyObject *index);

/**
 * Same as PySequence_Size, but throws a C++
 * exception on error.
 */
inline Py_ssize_t pysequence_size(PyObject *seq)
{
    Py_ssize_t s = PySequence_Size(seq);
    if (s == -1 && PyErr_Occurred()) {
        throw std::exception();
    }
    return s;
}

/**
 * Same as PyDict_GetItemString, but throws a
 * C++ exception on error.
 */
inline PyObject *pydict_getitemstring(PyObject *dp, const char *key)
{
    PyObject *result = PyDict_GetItemString(dp, key);
    if (result == NULL) {
        throw std::exception();
    }
    return result;
}

PyObject* intptr_array_as_tuple(int size, const intptr_t *array);

/**
 * Parses the axis argument, which may be either a single index
 * or a tuple of indices. They are converted into a boolean array
 * which is set to true whereever a reduction axis is provided.
 *
 * Returns the number of axes which were set.
 */
int pyarg_axis_argument(PyObject *axis, int ndim, dynd::dynd_bool *reduce_axes);

/**
 * Parses the error_mode argument. If it is None, returns
 * assign_error_default.
 */
dynd::assign_error_mode pyarg_error_mode(PyObject *error_mode_obj);

/**
 * Matches the input object against one of several
 * strings, returning the corresponding integer.
 */
int pyarg_strings_to_int(PyObject *obj, const char *argname, int default_value,
                const char *string0, int value0);

/**
 * Matches the input object against one of several
 * strings, returning the corresponding integer.
 */
int pyarg_strings_to_int(PyObject *obj, const char *argname, int default_value,
                const char *string0, int value0,
                const char *string1, int value1);

int pyarg_strings_to_int(PyObject *obj, const char *argname, int default_value,
                const char *string0, int value0,
                const char *string1, int value1,
                const char *string2, int value2);

int pyarg_strings_to_int(PyObject *obj, const char *argname, int default_value,
                const char *string0, int value0,
                const char *string1, int value1,
                const char *string2, int value2,
                const char *string3, int value3);

bool pyarg_bool(PyObject *obj, const char *argname, bool default_value);

uint32_t pyarg_access_flags(PyObject* obj);

} // namespace pydynd

#endif // _DYND__UTILITY_FUNCTIONS_HPP_
