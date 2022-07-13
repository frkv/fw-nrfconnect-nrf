.. _lib_identity_key:

Identity key
############

.. contents::
   :local:
   :depth: 2

The identity key library manages an asymmetric key used for identity services on devices with the Arm CryptoCell and KMU peripherals.

Functionality
*************

This library manages the identity keys, which are asymmetric keys intended to provide identity to specific devices. 
The identity key is expected to be unique and that is device-generated or otherwise provisioned in a secure manner during production.
The identity key is stored in an encrypted form inside the Key Management Unit (KMU) storage, in two reserved slots.
The HUK used to encrypt the identity key is the Master Key Encryption Key (MKEK).

.. caution::
   The identity key must not be shared. Leaking this leaks the identity of the dvice.

Prerequisites
*************

To use the identity key APIs, you must first generate or provision HUK keys on the device


API documentation
*****************

| Header file: :file:`include/identity_key.h`
| Source files: :file:`modules/lib/identity_key/`

.. doxygengroup:: identity_key
   :project: nrf
   :members:
