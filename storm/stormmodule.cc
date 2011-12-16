#include <Python.h>
#define STORM_MODULE
#include "stormmodule.h"

#ifdef __cplusplus
extern "C" {
#endif

static PyObject *StormError;

/*
 * Manipulating MPQ archives
 */

static PyObject * Storm_SFileOpenArchive(PyObject *self, PyObject *args) {
	HANDLE mpq = NULL;
	char *name;
	int priority = 0;
	int flags = 0;
	bool result;

	if (!PyArg_ParseTuple(args, "sii:SFileOpenArchive", &name, &priority, &flags)) {
		return NULL;
	}
	result = SFileOpenArchive(name, priority, MPQ_OPEN_READ_ONLY, &mpq);

	if (!result) {
		PyErr_SetString(StormError, "Error opening archive");
		return NULL;
	}

	return Py_BuildValue("l", mpq);
}

static PyObject * Storm_SFileCloseArchive(PyObject *self, PyObject *args) {
	HANDLE mpq = NULL;

	bool result;
	if (!PyArg_ParseTuple(args, "i:SFileCloseArchive", &mpq)) {
		return NULL;
	}
	result = SFileCloseArchive(mpq);

	if (!result) {
		PyErr_SetString(StormError, "Error closing archive");
		return NULL;
	}

	Py_RETURN_NONE;
}

/*
 * Using Patched archives
 */

/*
 * Reading Files
 */

static PyObject * Storm_SFileOpenFileEx(PyObject *self, PyObject *args) {
	HANDLE mpq = NULL;
	char *name;
	int scope = 0;
	HANDLE file = NULL;
	bool result;

	if (!PyArg_ParseTuple(args, "isi:SFileOpenFileEx", &mpq, &name, &scope)) {
		return NULL;
	}
	result = SFileOpenFileEx(mpq, name, scope, &file);

	if (!result) {
		PyErr_SetString(StormError, "Error opening file");
		return NULL;
	}

	return Py_BuildValue("l", file);
}

static PyObject * Storm_SFileHasFile(PyObject *self, PyObject *args) {
	HANDLE mpq = NULL;
	char *name;
	bool result;

	if (!PyArg_ParseTuple(args, "is:SFileHasFile", &mpq, &name)) {
		return NULL;
	}
	result = SFileHasFile(mpq, name);

	if (!result) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			Py_RETURN_FALSE;
		} else {
			PyErr_SetString(StormError, "Error searching for file");
			return NULL;
		}
	}

	Py_RETURN_TRUE;
}

static PyMethodDef StormMethods[] = {
	{"SFileOpenArchive",  Storm_SFileOpenArchive, METH_VARARGS, "Open a MPQ archive."},
	{"SFileCloseArchive",  Storm_SFileCloseArchive, METH_VARARGS, "Close a MPQ archive."},
	{"SFileOpenFileEx", Storm_SFileOpenFileEx, METH_VARARGS, "Open a file from a MPQ archive."},
	{"SFileHasFile", Storm_SFileHasFile, METH_VARARGS, "Check if a file exists within a MPQ archive."},
	{NULL, NULL, 0, NULL} /* Sentinel */
};

PyMODINIT_FUNC initstorm(void) {
	PyObject *m;

	m = Py_InitModule("storm", StormMethods);
	if (m == NULL) return;

	StormError = PyErr_NewException((char *)"storm.error", NULL, NULL);
	Py_INCREF(StormError);
	PyModule_AddObject(m, "error", StormError);
}

#ifdef __cplusplus
}
#endif