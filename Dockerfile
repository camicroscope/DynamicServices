FROM python
RUN apt-get install redis-server
RUN pip install Celery
