#!/usr/bin/env python3
"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: setup.py
Description: Production-level setup configuration for Blender Universal Asset Management System.
             Professional package configuration with comprehensive metadata, dependencies,
             and installation options for enterprise deployment.
"""

from setuptools import setup, find_packages
from pathlib import Path
import re
from typing import List, Dict, Any, Optional
import sys


def read_readme() -> str:
    """
    Read README.md file for long description.
    
    Returns:
        str: Content of README.md file or default description
        
    Raises:
        FileNotFoundError: If README.md doesn't exist
        UnicodeDecodeError: If README.md has encoding issues
    """
    readme_path = Path(__file__).parent / "README.md"
    if readme_path.exists():
        try:
            return readme_path.read_text(encoding='utf-8')
        except UnicodeDecodeError as e:
            print(f"Warning: Could not read README.md with UTF-8 encoding: {e}")
            return "Blender Universal Asset Management System"
    return "Blender Universal Asset Management System"


def read_requirements() -> List[str]:
    """
    Read requirements from requirements.txt file.
    
    Parses requirements.txt and extracts package names without version specifiers
    for use in setup.py dependencies.
    
    Returns:
        List[str]: List of package names
        
    Raises:
        FileNotFoundError: If requirements.txt doesn't exist
    """
    requirements_path = Path(__file__).parent / "requirements.txt"
    if requirements_path.exists():
        requirements = []
        try:
            with open(requirements_path, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#') and not line.startswith('#'):
                        # Remove version specifiers for setup.py
                        requirement = re.split(r'[<>=!~]', line)[0].strip()
                        if requirement and not requirement.startswith('#'):
                            requirements.append(requirement)
        except UnicodeDecodeError as e:
            print(f"Warning: Could not read requirements.txt with UTF-8 encoding: {e}")
            return []
        return requirements
    return []


def get_version() -> str:
    """
    Get version from version file or default.
    
    Reads version from VERSION file or returns default version.
    
    Returns:
        str: Version string
        
    Raises:
        FileNotFoundError: If VERSION file doesn't exist
    """
    version_path = Path(__file__).parent / "VERSION"
    if version_path.exists():
        try:
            return version_path.read_text().strip()
        except UnicodeDecodeError as e:
            print(f"Warning: Could not read VERSION file with UTF-8 encoding: {e}")
            return "1.0.0"
    return "1.0.0"


def validate_project_structure() -> None:
    """
    Validate that required project files exist.
    
    Checks for essential files and directories required for the package.
    
    Raises:
        FileNotFoundError: If required files are missing
    """
    required_files = [
        "README.md",
        "LICENSE",
        "VERSION",
        "requirements.txt",
        "utils.py"
    ]
    
    missing_files = []
    for file_name in required_files:
        if not (Path(__file__).parent / file_name).exists():
            missing_files.append(file_name)
    
    if missing_files:
        raise FileNotFoundError(f"Missing required files: {', '.join(missing_files)}")


# Project metadata
PROJECT_NAME = "blender-asset-manager"
PROJECT_VERSION = get_version()
PROJECT_DESCRIPTION = "Universal Blender Asset Management System"
PROJECT_LONG_DESCRIPTION = read_readme()
PROJECT_AUTHOR = "KleaSCM"
PROJECT_AUTHOR_EMAIL = "KleaSCM@gmail.com"
PROJECT_URL = "https://github.com/KleaSCM/blender-asset-manager"
PROJECT_DOWNLOAD_URL = f"{PROJECT_URL}/archive/v{PROJECT_VERSION}.tar.gz"
PROJECT_LICENSE = "MIT"
PROJECT_CLASSIFIERS = [
    "Development Status :: 5 - Production/Stable",
    "Intended Audience :: Developers",
    "Intended Audience :: End Users/Desktop",
    "License :: OSI Approved :: MIT License",
    "Operating System :: OS Independent",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Topic :: Multimedia :: Graphics :: 3D Modeling",
    "Topic :: Software Development :: Libraries :: Python Modules",
    "Topic :: System :: Filesystems",
    "Topic :: Utilities",
    "Framework :: Blender",
]

# Package configuration
PACKAGES = find_packages(include=['*'])
PACKAGE_DATA = {
    '': ['*.md', '*.txt', '*.json', '*.yaml', '*.yml'],
}

# Entry points for command-line tools
ENTRY_POINTS = {
    'console_scripts': [
        'blender-asset-audit=audit_assets:main',
        'blender-asset-test=test_utils:main',
        'blender-asset-validate=audit_and_test:main',
    ],
}

# Python version requirements
PYTHON_REQUIRES = ">=3.8"

# Install requirements
INSTALL_REQUIRES = read_requirements()

# Development requirements
EXTRAS_REQUIRE = {
    'dev': [
        'pytest>=6.2.0',
        'pytest-cov>=2.12.0',
        'pytest-mock>=3.6.0',
        'flake8>=4.0.0',
        'black>=21.0.0',
        'isort>=5.9.0',
        'mypy>=0.910',
        'pre-commit>=2.15.0',
        'tox>=3.24.0',
        'coverage>=6.0.0',
    ],
    'docs': [
        'sphinx>=4.0.0',
        'sphinx-rtd-theme>=1.0.0',
        'myst-parser>=0.15.0',
    ],
    'performance': [
        'psutil>=5.8.0',
        'memory-profiler>=0.60.0',
        'line-profiler>=3.3.0',
        'numba>=0.56.0',
        'cython>=0.29.0',
    ],
    'monitoring': [
        'watchdog>=2.1.0',
        'structlog>=21.1.0',
        'rich>=10.12.0',
    ],
    'full': [
        'Pillow>=9.0.0',
        'numpy>=1.21.0',
        'pandas>=1.3.0',
        'matplotlib>=3.5.0',
        'requests>=2.26.0',
        'aiohttp>=3.8.0',
        'pydantic>=1.8.0',
        'click>=8.0.0',
        'python-dotenv>=0.19.0',
    ],
}

# Keywords for package discovery
KEYWORDS = [
    "blender",
    "3d",
    "assets",
    "management",
    "universal",
    "automation",
    "workflow",
    "production",
    "enterprise",
]

# Project URLs for documentation and support
PROJECT_URLS = {
    "Bug Reports": f"{PROJECT_URL}/issues",
    "Source": PROJECT_URL,
    "Documentation": f"{PROJECT_URL}/docs",
    "Changelog": f"{PROJECT_URL}/blob/main/CHANGELOG.md",
}

# Setup configuration
def main() -> None:
    """
    Main setup function.
    
    Configures and runs the setup process with comprehensive validation
    and error handling for production deployment.
    """
    try:
        # Validate project structure
        validate_project_structure()
        
        # Run setup
        setup(
            name=PROJECT_NAME,
            version=PROJECT_VERSION,
            description=PROJECT_DESCRIPTION,
            long_description=PROJECT_LONG_DESCRIPTION,
            long_description_content_type="text/markdown",
            author=PROJECT_AUTHOR,
            author_email=PROJECT_AUTHOR_EMAIL,
            url=PROJECT_URL,
            download_url=PROJECT_DOWNLOAD_URL,
            license=PROJECT_LICENSE,
            classifiers=PROJECT_CLASSIFIERS,
            packages=PACKAGES,
            package_data=PACKAGE_DATA,
            include_package_data=True,
            python_requires=PYTHON_REQUIRES,
            install_requires=INSTALL_REQUIRES,
            extras_require=EXTRAS_REQUIRE,
            entry_points=ENTRY_POINTS,
            keywords=KEYWORDS,
            project_urls=PROJECT_URLS,
            zip_safe=False,
            platforms=["any"],
            provides=["blender_asset_manager"],
            requires_python=">=3.8",
            setup_requires=[
                "setuptools>=45.0.0",
                "wheel>=0.37.0",
            ],
            test_suite="tests",
            tests_require=[
                "pytest>=6.2.0",
                "pytest-cov>=2.12.0",
                "pytest-mock>=3.6.0",
            ],
            cmdclass={},
            options={
                'bdist_wheel': {
                    'universal': True,
                },
            },
        )
        
    except FileNotFoundError as e:
        print(f"❌ Setup failed: {e}")
        print("💡 Please ensure all required files exist before running setup")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Unexpected error during setup: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main() 