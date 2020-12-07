#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <math.h>

static double
get_elem(PyObject* matrix, size_t row, size_t column) {
  return PyFloat_AsDouble(
      PyList_GetItem(PyList_GetItem(matrix, row), column)
      );
}

static double*
pylist_matrix_to_array(PyObject* matrix, size_t size) {
  double* result = calloc(size*size, sizeof(double));
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      result[i*size + j] = get_elem(matrix, i, j);
    }
  }
  return result;
}

static double*
multiply_matrix(const double* left, const double* right, size_t size) {
  double* result = calloc(size*size, sizeof(double));
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      double dot_sum = 0;
      for (int k = 0; k < size; ++k) {
        dot_sum += left[i*size + k] * right[k*size + j];
      }
      result[i*size + j] = dot_sum;
    }
  }
  return result;
}

static PyObject*
arr_to_pylist_matrix(double* arr, size_t size) {
  PyObject* result = PyList_New(size);
  for (int i = 0; i < size; ++i) {
    PyObject* row = PyList_New(size);
    for (int j = 0; j < size; ++j) {
      PyList_SetItem(row, j, PyFloat_FromDouble(arr[i*size + j]));
    }
    PyList_SetItem(result, i, row);
  }
  return result;
}

static PyObject*
matrix_dot(PyObject* self, PyObject *args) {
  PyObject* matrix_1 = NULL;
  PyObject* matrix_2 = NULL;
  size_t size = 0;
  if(!PyArg_ParseTuple(args, "KOO", &size, &matrix_1, &matrix_2)) {
    return NULL;
  }
  size_t matrix1_real_size = PyList_Size(matrix_1);
  size_t matrix2_real_size = PyList_Size(matrix_2);
  size_t min_size = fmin(size,
                         fmin(matrix1_real_size, matrix2_real_size));
  double* left_matrix = pylist_matrix_to_array(matrix_1, min_size);
  double* right_matrix = pylist_matrix_to_array(matrix_2, min_size);
  double* mul_result = multiply_matrix(left_matrix, right_matrix, min_size);
  PyObject* result = arr_to_pylist_matrix(mul_result, min_size);
  free(left_matrix);
  free(right_matrix);
  free(mul_result);
  return result;
}

static PyMethodDef methods[] = {
    {
        .ml_name = "dot",
        .ml_meth = matrix_dot,
        .ml_flags = METH_VARARGS,
        .ml_doc = "Returns product of two matrices"
    },
    {NULL, NULL, 0, NULL}
};

static PyModuleDef matrix = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "matrix",
    .m_size = -1,
    .m_methods = methods,
};

PyMODINIT_FUNC PyInit_matrix(void) {
  return PyModule_Create(&matrix);
}
