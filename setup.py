from setuptools import setup

setup(
    name = 'automaton',
    version = '0.0.1',
    author = 'Matt Fichman',
    author_email = 'matt.fichman@gmail.com',
    description = ('Server provisioning tool'),
    license = 'MIT',
    keywords = ('provisioning'),
    url = 'http://github.com/mfichman/automaton',
    packages = ['automaton', 'automaton.rule', 'automaton.module'],
    entry_points = {
        'console_scripts': (
            'automaton = automaton.__main__:main'
        )
    }
)
    
    
    
    
