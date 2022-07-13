.. _hw_unique_key_usage:

Identity Key Usage
##################

.. contents::
   :local:
   :depth: 2

This sample shows how to use a previously written ientity key with psa_crypto APIs.

Requirements
************

The sample supports the following development kits:

.. table-from-sample-yaml::

.. include:: /includes/tfm.txt

Overview
********

To be filled in

Configuration
*************

|config|

FEM support
===========

.. include:: /includes/sample_fem_support.txt

Building and running
********************

.. |sample path| replace:: :file:`samples/keys/identity_key_usage`

.. include:: /includes/build_and_run.txt

Testing
=======

|test_sample|

      1. |connect_terminal_specific|
      #. Reset the kit.
      #. Observe the following output:

         .. code-block:: console

             Success!

         If an error occurs, the sample prints a message and raises a kernel panic.

   If an error occurs, the sample prints a message and raises a kernel panic.

Dependencies
************

This sample uses the following libraries:

* :ref:`lib_hw_unique_key`
* :ref:`nrf_security`
* :ref:`Trusted Firmware-M <ug_tfm>`, when using a non-secure board.
