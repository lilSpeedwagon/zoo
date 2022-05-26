import pytest

def test_create(api_config):
    response = api_config.post(
        '/api/v1/api-config/create',
        {
            'schema': {
                'name': 'config',
                'description': 'config',
                'schema': {},
            },
        },
    )
    assert(response.status_code == 200)
    assert(response.text == 'OK')
