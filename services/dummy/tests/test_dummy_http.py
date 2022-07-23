import pytest


def test_ping(dummy_service):
    response = dummy_service.get('/ping')
    assert(response.status_code == 200)
    assert(response.text == 'OK')
    assert(response.headers == {
        'Content-Length': '2',
        'Content-Type': 'application/json',
        'Server': 'SelfMadeZoo Http 0.1',
    })

def test_get(dummy_service):
    result = dummy_service.get('/get')
    assert(result.status_code == 200)
    assert(result.json() == {'test_data': 'hello world'})

def test_get_with_params(dummy_service):
    result = dummy_service.get('/get_parametrized?key1=value1&key2=value2')
    assert(result.status_code == 200)
    assert(result.json() == {'params': {'key1': 'value1', 'key2': 'value2'}})

def test_not_found(dummy_service):
    result = dummy_service.get('/unknown')
    assert(result.status_code == 404)
    assert(result.text == 'Not found.')

def test_wrong_method(dummy_service):
    result = dummy_service.post('/get')
    assert(result.status_code == 404)
    assert(result.text == 'Not found.')

def test_post(dummy_service):
    result = dummy_service.post(
        '/post', {'number': 1, 'string': 'hello', 'flag': True})
    assert(result.status_code == 200)
    assert(result.json() == {'result': 'success'})

@pytest.mark.parametrize('json', [(None), ({'number': 'string'}), ({'number': 1})])
def test_post_bad_request(dummy_service, json):
    result = dummy_service.post('/post', json)
    assert(result.status_code == 400)
    assert(result.text == 'Bad request')
