from typing import Optional

import pytest

class MockAny:
    # Returns True on assertion with any other object.
    # May be used for assertion non-determined data
    # (random values, date, time, etc.).

    def __eq__(self, other):
        return True


def _create_api_config(service, name: str, schema: dict = {},
                       description: str='') -> dict:
    response = service.post(
        '/api/v1/api-config/create',
        {
            'schema': {
                'name': name,
                'description': description,
                'schema': schema,
            },
        },
    )
    expected_response = {
        'name': name,
        'description': description,
        'schema': schema,
        'id': MockAny(),
        'author': 'author',
        'created': MockAny(),
        'updated': MockAny(),
    }
    assert(response.status_code == 200)
    result = response.json()
    assert(result == expected_response)
    return result


def test_list_empty(api_config):
    response = api_config.get(f'/api/v1/api-config/list')
    expected_response = {'items': None}
    assert(response.status_code == 200)
    assert(response.json() == expected_response)


def test_create(api_config):
    schema={'key': 'value', 'value': 'key'}
    _create_api_config(api_config, name='config',
                       description='config description', schema=schema)


def test_get(api_config):
    schema={'key': 'value', 'value': 'key'}
    created = _create_api_config(api_config, name='config',
                                 description='config description', schema=schema)
    id = created['id']

    response = api_config.get(f'/api/v1/api-config/get?id={id}')
    expected_response = {
        'name': 'config',
        'description': 'config description',
        'schema': {
            'key': 'value',
            'value': 'key',
        },
        'id': id,
        'author': 'author',
        'created': created['created'],
        'updated': created['updated'],
    }
    assert(response.status_code == 200)
    assert(response.json() == expected_response)


def test_get_missing(api_config):
    response = api_config.get('/api/v1/api-config/get?id=10')
    assert(response.status_code == 404)
    assert(response.text == 'API config with id \'10\' not found')


@pytest.mark.parametrize('id', [('-1'), ('abc'), (' ')])
def test_get_invalid_id(api_config, id):
    response = api_config.get(f'/api/v1/api-config/get?id={id}')
    assert(response.status_code == 400)
    assert(response.text == 'Parameter \'id\' is invalid')


def test_list(api_config):
    response = api_config.get(f'/api/v1/api-config/list')
    expected_response = {
        'items': [
            {
                'name': 'config',
                'description': 'config description',
                'schema': {
                    'key': 'value',
                    'value': 'key',
                },
                'id': MockAny(),
                'author': 'author',
                'created': MockAny(),
                'updated': MockAny(),
            },
            {
                'name': 'config',
                'description': 'config description',
                'schema': {
                    'key': 'value',
                    'value': 'key',
                },
                'id': MockAny(),
                'author': 'author',
                'created': MockAny(),
                'updated': MockAny(),
            },
        ],
    }
    assert(response.status_code == 200)
    assert(response.json() == expected_response)