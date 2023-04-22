

def test_dummy_counter(dummy_service):
    for i in range(5):
        response = dummy_service.post('/count')
        assert(response.status_code == 200)
        assert(response.json() == {'result': i + 1})


def test_dummy_dict(dummy_service):
    data = {'key': 'key1', 'value': 'value1'}
    response = dummy_service.post('/dict-add', data)
    assert(response.status_code == 200)
    assert(response.text == '')

    data = {'key': 'key2', 'value': 'value2'}
    response = dummy_service.post('/dict-add', data)
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.get('/dict-get')
    assert(response.status_code == 400)
    assert(response.text == 'Missing required param \'key\'')

    response = dummy_service.get('/dict-get?key=key1')
    assert(response.status_code == 200)
    assert(response.json() == {'value': 'value1'})

    response = dummy_service.get('/dict-get?key=key2')
    assert(response.status_code == 200)
    assert(response.json() == {'value': 'value2'})

    response = dummy_service.get('/dict-get?key=unknown')
    assert(response.status_code == 200)
    assert(response.json() == {'value': None})


def test_reset_by_name(dummy_service):
    # counter
    for i in range(3):
        response = dummy_service.post('/count')
        assert(response.status_code == 200)
        assert(response.json() == {'result': i + 1})
    
    response = dummy_service.reset('dummy-counter')
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.post('/count')
    assert(response.status_code == 200)
    assert(response.json() == {'result': 1})

    # dict
    response = dummy_service.post('/dict-add', {'key': 'key', 'value': 'value'})
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.reset('dummy-dict')
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.get('/dict-get?key=key')
    assert(response.status_code == 200)
    assert(response.json() == {'value': None})


def test_reset_all(dummy_service):
    for i in range(3):
        response = dummy_service.post('/count')
        assert(response.status_code == 200)
        assert(response.json() == {'result': i + 1})
    
    response = dummy_service.post('/dict-add', {'key': 'key', 'value': 'value'})
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.reset()
    assert(response.status_code == 200)
    assert(response.text == '')

    response = dummy_service.post('/count')
    assert(response.status_code == 200)
    assert(response.json() == {'result': 1})

    response = dummy_service.get('/dict-get?key=key')
    assert(response.status_code == 200)
    assert(response.json() == {'value': None})


def test_reset_missing_component(dummy_service):
    name = 'unknown'
    response = dummy_service.reset(name)
    assert(response.status_code == 404)
    assert(response.text == f'Component \'{name}\' not found')
