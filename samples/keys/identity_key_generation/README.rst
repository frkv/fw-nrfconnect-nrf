.. _identity_key_generate:

Identity key generation
#######################

.. contents::
   :local:
   :depth: 2

This sample generates a device-specific Identity key and uses the MKEK key to encrypt it for storage in the Key Management Unit (KMU).

Requirements
************

The sample supports the following development kits:

.. table-from-sample-yaml::

Overview
********

The identity key requires usage of :ref:`lib_hw_unique_key` library to the prerequisite Master Key Encryption Key (MKEK) HUK.

This sample is expected to be run e.g. for attestation key support in TF-M projects.
It will save the randomly generated HUKs and identity key the device.

Configuration
*************

|config|

FEM support
===========

.. include:: /includes/sample_fem_support.txt

Building and running
********************

.. |sample path| replace:: :file:`samples/keys/identity_key_generate`

.. include:: /includes/build_and_run.txt

Testing
=======

|test_sample|

      1. |connect_terminal_specific|
      #. Reset the kit.
      #. Observe the following output:

         .. code-block:: console

             Writing the identity key to KMU
             Reading back the identity key to check
             Success!

         If an error occurs, the sample prints a message and raises a kernel panic.

Dependencies
*************

This sample uses the following libraries:

* :ref:`lib_hw_unique_key`
