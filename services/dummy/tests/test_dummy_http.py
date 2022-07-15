import pytest
import requests

PORT_ = 1111
LOCALHOST_ = 'localhost'
HOST_ = f'http://{LOCALHOST_}:{PORT_}'

class MockAny:
    # Returns True on assertion with any other object.
    # May be used for assertion non-determined data
    # (random values, date, time, etc.).

    def __eq__(self, other):
        return True


def test_ping():
    response = requests.get(f'{HOST_}/ping')
    assert(response.status_code == 200)
    assert(response.text == 'OK')
    assert(response.headers == {
        'Content-Length': '2',
        'Content-Type': 'application/json',
        'Server': 'SelfMadeZoo Http 0.1',
    })

def test_get():
    result = requests.get(f'{HOST_}/get')
    assert(result.status_code == 200)
    assert(result.json() == {'test_data': 'hello world'})

def test_get_with_params():
    result = requests.get(f'{HOST_}/get_parametrized?key1=value1&key2=value2')
    assert(result.status_code == 200)
    assert(result.json() == {'params': {'key1': 'value1', 'key2': 'value2'}})

def test_not_found():
    result = requests.get(f'{HOST_}/unknown')
    assert(result.status_code == 404)
    assert(result.text == 'Not found.')

def test_wrong_method():
    result = requests.post(f'{HOST_}/get')
    assert(result.status_code == 404)
    assert(result.text == 'Not found.')

def test_post():
    result = requests.post(f'{HOST_}/post', 
                           json={'number': 1, 'string': 'hello', 'flag': True})
    assert(result.status_code == 200)
    assert(result.json() == {'result': 'success'})

@pytest.mark.parametrize('json', [(None), ({'number': 'string'}), ({'number': 1})])
def test_post_bad_request(json):
    result = requests.post(f'{HOST_}/post', json=json)
    assert(result.status_code == 400)
    assert(result.text == 'Bad request')



# TODO
# get with unused params
# get with bad param 400
# post
# post with body
# post with bad body
# not found with wrong method
# 
# check headers, version etc. ???????



