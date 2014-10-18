from setuptools import setup, find_packages

setup(
    name = 'automaton',
    version = '0.0.1',
    author = 'Matt Fichman',
    author_email = 'matt.fichman@gmail.com',
    description = ('Server provisioning tool'),
    license = 'MIT',
    keywords = ('provisioning'),
    url = 'http://github.com/mfichman/automaton',
    packages = find_packages(),
    install_requires = ('paramiko',),
    entry_points = {
        'console_scripts': (
            'automaton = automaton.__main__:main'
        )
    }
)
    
    
    
    
