# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

"""
DSPLIB_idat_gen package initialization.
This package contains Python models for various DSPLIB kernels.
This file dynamically imports all DSPLIB modules and their classes in the package directory.
"""

import os
import importlib

# Initialize the __all__ list
__all__ = []

# Get the package directory
package_dir = os.path.dirname(__file__)

# Dynamically import all subdirectories that are modules (contain __init__.py)
for item in os.listdir(package_dir):
    item_path = os.path.join(package_dir, item)
    # Check if it's a directory and starts with DSPLIB_
    if os.path.isdir(item_path) and item.startswith("DSPLIB_"):
        # Check if it has an __init__.py file (making it a proper module)
        if os.path.exists(os.path.join(item_path, "__init__.py")):
            # Import the module
            module_name = item
            try:
                # Import the module
                module = importlib.import_module(f".{module_name}", __name__)
                globals()[module_name] = module

                # Add the module name to __all__
                __all__.append(module_name)

                # Import all classes from the module and add them to globals and __all__
                if hasattr(module, "__all__"):
                    for class_name in module.__all__:
                        # Get the class from the module
                        cls = getattr(module, class_name)
                        # Add it to globals so it can be imported directly from this package
                        globals()[class_name] = cls
                        # Add it to __all__
                        __all__.append(class_name)

            except ImportError as e:
                print(f"Warning: Could not import {module_name}: {e}")
