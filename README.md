## DynamicServices

### Overview
DynamicServices is a suit of web-services to run containerized algorithms on large bio-medical image data. It can used to run the algorithm on a region of interest (ROI) tile or on the whole image.  


### Dynamic registration of tasks

Passing a function to be executed as a celery task as a string

```
@app.task
def dynamic_tasks(funcname, funccode, *args, **kwargs):
    try:
        ns = {}
        code = compile(funccode, funcname, 'exec')
        eval(code, ns, ns)
        logger.info('execute %r with args %r, %r', funcname, args, kwargs)
        return ns['task'](*args, **kwargs)
    except IOError:
        logger.error("Error loading the dynamic function from text %s", funcname)

```
