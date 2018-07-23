"""
WSGI config for muffler_sim project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/2.0/howto/deployment/wsgi/
"""

# activate virtualenv
python_home = '/home/scallion/Documents/project/project_active/muffler_sim/env'
activate_this = python_home + '/bin/activate_this.py'
exec(open(activate_this).read(), {'__file__':activate_this})

# see if the python version has been changed
# import sys
# print(sys.stderr, sys.version)

import os

from django.core.wsgi import get_wsgi_application

# add project root & project lib to sys.path
PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
import sys
sys.path.insert(0, PROJECT_DIR)
sys.path.insert(0, os.path.join(PROJECT_DIR, 'libs/zycwn_lib'))

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "muffler_sim.settings")

application = get_wsgi_application()
