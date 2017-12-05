from __future__ import absolute_import, unicode_literals

from celery import Celery

app = Celery('tasks', broker='pyamqp://guest@localhost//')

app.conf.update(
    result_expires=3600,
)

if __name__ == '__main__':
    app.start()
