Blaze-Local NDArray
===================

In this preview release of blaze-local, there is an `ndarray` object
which hold a multi-dimensional array of elements of a specific `dtype`.
Future versions of this library will likely explore some different
ways of organizing the data model, but something like the ndarray
will stay.

One of the important features of this library is interoperability with
Numpy that is as seamless as possible. When a dtype is compatible across
both systems, ndarrays from Numpy and blaze-local will implicitly move
between the libraries.

Constructing From Python Scalars
--------------------------------

Currently, scalars are always zero-dimensional arrays in the system.
This is likely to change in a future preview release.

There's a difference with Numpy in the default treatment of integers.
In Numpy, the C/C++ ``long`` type is used, which is 32-bits on 32-bit
platforms and on 64-bit Windows, and which is 64-bits on Linux and OS X.
In blaze-local, if the Python integer fits in 32-bits, it will always
be initialized by default as a 32-bit integer.

.. code-block:: python

    >>> nd.ndarray(0)
    nd.ndarray(0, int32)

    >>> nd.ndarray(3.14)
    nd.ndarray(3.14, float64)

    >>> nd.ndarray(1 + 1j)
    nd.ndarray((1,1), complex<float64>)

Strings default to `blockref` strings, which are variable-sized strings.
The support for them is still preliminary, but some basic functionality
like converting between different unicode encodings is implemented.

One thing to note is that strings and unicode scalars are imported as
a view into the Python object's data. Because the Python object is immutable,
the ndarray also is flagged as immutable.

.. code-block:: python

    >>> nd.ndarray('testing')
    nd.ndarray("testing", string<ascii>)

    >>> nd.ndarray(u'testing')
    nd.ndarray("testing", string<ucs_2>)

Constructing from Python Lists
------------------------------

Similar to in Numpy, ndarrays can be constructed from lists of
objects. This code does not try to be as clever as Numpy, and
will fail if something is inconsistent in the input data.

.. code-block:: python

    >>> nd.ndarray([True, False])
    nd.ndarray([true, false], bool)

    >>> nd.ndarray([1,2,3])
    nd.ndarray([1, 2, 3], int32)

    >>> nd.ndarray([[1.0,0],[0,1.0]])
    nd.ndarray([[1, 0], [0, 1]], float64)

    >>> nd.ndarray(["testing", "one", u"two", "three"])
    nd.ndarray(["testing", "one", "two", "three"], string<ucs_2>)

Converting to Python Types
--------------------------

To convert back into native Python objects, there is an ``as_py()``
function.

.. code-block:: python

    >> x = nd.ndarray([True, False])
    >> x.as_py()
    [True, False]

    >> x = nd.ndarray("testing")
    >> x.as_py()
    u'testing'

Constructing from Numpy Scalars
-------------------------------

Numpy scalars are also supported as input, and the dtype is preserved
in the conversion.

.. code-block:: python

    >>> x = np.bool_(False)
    >>> nd.ndarray(x)
    nd.ndarray(false, bool)

    >>> x = np.int16(1000)
    >>> nd.ndarray(x)
    nd.ndarray(1000, int16)

    >>> x = np.complex128(3.1)
    >>> nd.ndarray(x)
    nd.ndarray((3.1,0), complex<float64>)

Constructing from Numpy Arrays
------------------------------

When the dtype is supported by blaze-local, Numpy arrays can
be converted into ndarrays. The resulting array points at the same
data the Numpy array used.

.. code-block:: python

    >>> x = np.arange(6.).reshape(3,2)
    >>> nd.ndarray(x)
    nd.ndarray([[0, 1], [2, 3], [4, 5]], float64)

    >>> x = np.array(['testing', 'one', 'two', 'three'])
    >>> nd.ndarray(x)
    nd.ndarray(["testing", "one", "two", "three"], fixedstring<ascii,7>)


Converting to Numpy Arrays
--------------------------

To support naturally feeding data into Numpy operations, the
Numpy array interface is used via the C struct PyArrayInterface.
This means Numpy operations will work on ndarrays with compatible
dtypes.

.. code-block:: python

    >>> x = nd.ndarray([1, 2, 3.5])
    >>> np.square(x)
    array([  1.  ,   4.  ,  12.25])

