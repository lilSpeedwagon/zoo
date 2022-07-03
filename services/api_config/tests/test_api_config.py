
import pytest

class MockAny:
    # Returns True on assertion with any other object.
    # May be used for assertion non-determined data
    # (random values, date, time, etc.).

    def __eq__(self, other):
        return True


def _create_api_config(
    service, name: str, schema: dict = {'key': 'value', 'value': 'key'},
    description: str=''
) -> dict:
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


def test_create(api_config):
    schema={'key': 'value', 'value': 'key'}
    _create_api_config(api_config, name='config',
                       description='config description', schema=schema)


def test_get(api_config):
    created = _create_api_config(api_config, name='config',
                                 description='config description')
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
    response = api_config.get('/api/v1/api-config/get?id=0')
    assert(response.status_code == 404)
    assert(response.text == 'API config with id \'0\' not found')


@pytest.mark.parametrize('id', [('-1'), ('abc'), (' ')])
def test_get_invalid_id(api_config, id):
    response = api_config.get(f'/api/v1/api-config/get?id={id}')
    assert(response.status_code == 400)
    assert(response.text == 'Parameter \'id\' is invalid')


def test_list(api_config):
    ids = [
        _create_api_config(api_config, name='config1', description='config1')['id'],
        _create_api_config(api_config, name='config2', description='config2')['id'],
        _create_api_config(api_config, name='config3', description='config3')['id'],
    ]

    response = api_config.get(f'/api/v1/api-config/list')
    expected_response = {
        'items': [
            {
                'name': 'config3',
                'description': 'config3',
                'schema': {
                    'key': 'value',
                    'value': 'key',
                },
                'id': ids[2],
                'author': 'author',
                'created': MockAny(),
                'updated': MockAny(),
            },
            {
                'name': 'config2',
                'description': 'config2',
                'schema': {
                    'key': 'value',
                    'value': 'key',
                },
                'id': ids[1],
                'author': 'author',
                'created': MockAny(),
                'updated': MockAny(),
            },
            {
                'name': 'config1',
                'description': 'config1',
                'schema': {
                    'key': 'value',
                    'value': 'key',
                },
                'id': ids[0],
                'author': 'author',
                'created': MockAny(),
                'updated': MockAny(),
            },
        ],
    }
    assert(response.status_code == 200)
    assert(response.json() == expected_response)


def test_list_empty(api_config):
    response = api_config.get(f'/api/v1/api-config/list')
    expected_response = {'items': None}
    assert(response.status_code == 200)
    assert(response.json() == expected_response)


def test_delete(api_config):
    created = _create_api_config(api_config, name='config',
                                 description='config description')
    id = created['id']

    response = api_config.post(f'/api/v1/api-config/delete?id={id}')
    assert(response.status_code == 200)
    assert(response.json() == created)

    response = api_config.get(f'/api/v1/api-config/get?id={id}')
    assert(response.status_code == 404)
    assert(response.text == f'API config with id \'{id}\' not found')


@pytest.mark.parametrize('id', [('-1'), ('abc'), (' ')])
def test_delete_invalid_id(api_config, id):
    response = api_config.post(f'/api/v1/api-config/delete?id={id}')
    assert(response.status_code == 400)
    assert(response.text == 'Parameter \'id\' is invalid')


def test_delete_missing(api_config):
    response = api_config.post('/api/v1/api-config/delete?id=0')
    assert(response.status_code == 404)
    assert(response.text == 'API config with id \'0\' not found')


def test_update(api_config):
    created = _create_api_config(api_config, name='config',
                                 description='config description')
    id = created['id']

    response = api_config.post(
        '/api/v1/api-config/update',
        {
            'id': id,
            'schema': {
                'name': 'new name',
                'description': 'new description',
                'schema': {'new_key': 'new_value'},
            },
        }
    )
    expected_config = {
        'id': created['id'],
        'created': created['created'],
        'updated': MockAny(),
        'author': created['author'],
        'name': 'new name',
        'description': 'new description',
        'schema': {'new_key': 'new_value'},
    }
    assert(response.status_code == 200)
    assert(response.json() == expected_config)

    response = api_config.get(f'/api/v1/api-config/get?id={id}')
    assert(response.status_code == 200)
    assert(response.json() == expected_config)


def test_update_bad_request(api_config):
    created = _create_api_config(api_config, name='config',
                                 description='config description')
    response = api_config.post(
        '/api/v1/api-config/update',
        {
            'id': created['id'],
            'schema': {'name': 'new name'},
        }
    )
    assert(response.status_code == 400)


def test_update_missing(api_config):
    response = api_config.post(
        '/api/v1/api-config/update',
        {
            'id': 0,
            'schema': {
                'name': 'new name',
                'schema': {'new_key': 'new_value'},
            },
        },
    )
    assert(response.status_code == 404)
    assert(response.text == 'API config with id \'0\' not found')

