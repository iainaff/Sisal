#include "Python.h"
#include "../mySAL/ParserControl.h"

/**************************************************************************/
/* LOCAL  **************       generateToken       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
static void* generateToken(void* master,
			   const char* tokenText,
			   const char* fileName,
			   unsigned long startLine,
			   unsigned long startColumn,
			   unsigned long endLine,
			   unsigned long endColumn) {
    PyObject* self = (PyObject*)master;
    PyObject* result = 0;

    /* ----------------------------------------------- */
    /* We give up if we tripped an exception	       */
    /* ----------------------------------------------- */
    if ( PyErr_Occurred() ) return 0;

    /* ----------------------------------------------- */
    /* The master object has a method for generating   */
    /* tokens					       */
    /* ----------------------------------------------- */
    if ( !self ) return 0;
    result = PyObject_CallMethod(self,"token","ssllll",tokenText,fileName,startLine,startColumn,endLine,endColumn);
    if ( PyErr_Occurred() ) return 0;
    return result;
}

/**************************************************************************/
/* LOCAL  **************      reductionAction      ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
static void* reductionAction(void* master,
			     const char* lhs,
			     unsigned int rule,
			     void** rhs,
			     unsigned int rhsSize) {
    PyObject* self = (PyObject*)master;
    int i;
    PyObject* method = 0;
    PyObject* args = 0;
    PyObject* result = 0;
    char methodName[1024];

    /* ----------------------------------------------- */
    /* We give up if we tripped an exception	       */
    /* ----------------------------------------------- */
    if ( PyErr_Occurred() ) return 0;

    /* ----------------------------------------------- */
    /* We call the master method/function with args    */
    /* Method is either called NT_3 or NT              */
    /* ----------------------------------------------- */
    if ( !self ) return 0;
    sprintf(methodName,"%s_%d",lhs,rhsSize);

    fprintf(stderr,"%s ==>",methodName);
    fflush(stderr);

    method = PyObject_GetAttrString(self,methodName);
    if ( PyErr_Occurred() ) {
	PyErr_Clear();
	method = PyObject_GetAttrString(self,(char*)lhs);
    }
    if ( PyErr_Occurred() ) return 0;

    /* ----------------------------------------------- */
    /* Build up the argument tuple		       */
    /* ----------------------------------------------- */
    args = PyTuple_New(rhsSize);
    for(i=0; i < rhsSize; ++i) {
	PyTuple_SET_ITEM(args,i,rhs[i]);
    }

    /* ----------------------------------------------- */
    /* Apply the function			       */
    /* ----------------------------------------------- */
    result = PyObject_CallObject(method,args);
    if ( PyErr_Occurred() ) result = 0;
	
    Py_XDECREF(args);
    Py_XDECREF(method);

    PyObject_Print(result,stderr,0);
    fputs("\n",stderr);
    fflush(stderr);
    return result;
}

/**************************************************************************/
/* LOCAL  **************      reductionAction      ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
static void* errorAction(void* master,
			     const char* msg,
			     const char* lhs,
			     unsigned int rule,
			     void* rhs) {
    PyObject* self = (PyObject*)master;
    PyObject* result = 0;
    char methodName[1024];

    /* ----------------------------------------------- */
    /* We give up if we tripped an exception	       */
    /* ----------------------------------------------- */
    if ( PyErr_Occurred() ) return 0;

    /* ----------------------------------------------- */
    /* We call the master method/function with args    */
    /* Method is either called NT_3 or NT              */
    /* ----------------------------------------------- */
    if ( !self ) return 0;
    sprintf(methodName,"%s_error",lhs);
    fputs(methodName,stderr);
    result = PyObject_CallMethod(self,methodName,"O",rhs);
    if ( PyErr_Occurred() ) return 0;

    return result;
}

static int errorHandler(void* master,const char* msg) {
    PyObject* self = (PyObject*)master;
    fprintf(stderr,"oops %s\n",msg);
    return 1;
}

static int inputRoutine(void* master, char* buf, int max_chars) {
    PyObject* self = (PyObject*)master;
    PyObject* result = 0;
    char* str = 0;
    int status = 0;


    /* ----------------------------------------------- */
    /* Quit reading if a python exception thrown (also */
    /* we don't want to mask a user exception)	       */
    /* ----------------------------------------------- */
    if ( PyErr_Occurred() ) return 0;

    result = PyObject_CallMethod(self,"read","i",max_chars); /* Owned */
    if ( PyErr_Occurred() ) {
	PyErr_Clear();
    } else if ( PyString_Check(result) ) {
	str = PyString_AsString(result);
	if ( PyErr_Occurred() ) {
	    PyErr_Clear();
	} else {
	    strncpy(buf,str,max_chars);
	    status = strlen(str);
	}
    }

    Py_XDECREF(result);
    return status;
}

static PyObject* parser(PyObject* self, PyObject* args) {
    PyObject* master = 0;
    PyObject* name = 0;
    PyObject* result = 0;
    char* filename = 0;
    int status = 0;

    /* ----------------------------------------------- */
    /* We need to crack out the file, parser, and the  */
    /* optional filename			       */
    /* ----------------------------------------------- */
    if ( !PyArg_ParseTuple(args,"O",&master) ) {
	return 0;
    }

    /* ----------------------------------------------- */
    /* Grab name from file object (if defaulted)       */
    /* ----------------------------------------------- */
    if ( !filename ) {
	name = PyObject_CallMethod(master,"name",0); /* Owned */
	if ( PyErr_Occurred() || !name ) {
	    PyErr_Clear();
	    filename = "<unknown>";
	} else {
	    filename = PyString_AsString(name);
	    if ( PyErr_Occurred() || !filename ) {
		PyErr_Clear();
		filename = "<unknown>";
	    }
	}
    }

    fprintf(stderr,"filename is %s\n",filename);

    status = yyProgrammedParse(master,
			       filename,
			       generateToken,
			       reductionAction,
			       errorAction,
			       errorHandler,
			       inputRoutine,
			       &result);


    /* ----------------------------------------------- */
    /* cleanup					       */
    /* ----------------------------------------------- */
    Py_XDECREF(name);

    /* ----------------------------------------------- */
    /* We examine the return value to see if the Python*/
    /* bombed.					       */
    /* ----------------------------------------------- */
    if ( PyErr_Occurred() ) return 0;

    /* ----------------------------------------------- */
    /* Maybe we recovered and the status was bad       */
    /* ----------------------------------------------- */
    fprintf(stderr,"status is %d\n",status);

    return result;
}

static PyMethodDef methods[] = {
    {"parse",parser, METH_VARARGS, "create parse tree"},
    {0,0}
};

void initmysal() {
    PyObject* module = 0;
    module = Py_InitModule("mysal",methods);
}
