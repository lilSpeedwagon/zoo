
def test_ping(api_config):
    response = api_config.get('/ping')
    assert(response.status_code == 200)
    assert(response.text == 'OK')
