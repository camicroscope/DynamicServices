FROM python
RUN apt-get -qq -y update
RUN apt-get -y upgrade
RUN apt-get -y install redis-server
RUN pip install Celery
