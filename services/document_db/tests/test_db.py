
import pytest

from zootest.mocks import MockAny

def _create_document(document_db, name: str = 'doc', owner: str = 'me',
                     namespace: str = '', payload: str = '') -> dict:
    document = {
        'name': name,
        'owner': owner,
        'namespace': namespace,
        'payload': payload,
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 200)
    document = response.json()
    assert(document == {
        'id': MockAny(),
        'created': MockAny(),
        'updated': MockAny(),
        'name': name,
        'owner': owner,
        'namespace': namespace,
    })
    return document


def test_create(document_db):
    document = {
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
        'payload': '{\'key\': \'value\', \'key2\': \'value2\'}',
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': MockAny(),
        'created': MockAny(),
        'updated': MockAny(),
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    })


def test_without_payload(document_db):
    document = {
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 400)


def test_create_empty_payload(document_db):
    document = {
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
        'payload': '',
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': MockAny(),
        'created': MockAny(),
        'updated': MockAny(),
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    })


def test_create_twice(document_db):
    document = {
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
        'payload': '',
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': MockAny(),
        'created': MockAny(),
        'updated': MockAny(),
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    })
    first_id = response.json()['id']

    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': MockAny(),
        'created': MockAny(),
        'updated': MockAny(),
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    })
    assert(first_id != response.json()['id'])
    

def test_get(document_db):
    doc = _create_document(document_db, payload='payload')
    id = doc['id']
    response = document_db.get(f'/api/v1/documents/get?id={id}')
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': id,
        'created': doc['created'],
        'updated': doc['updated'],
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
        'payload': 'payload',
    })


def test_get_missing(document_db):
    response = document_db.get(f'/api/v1/documents/get?id=0')
    assert(response.status_code == 404)
    assert(response.text == 'Document with id=\'0\' not found')


def test_get_bad_request(document_db):
    response = document_db.get(f'/api/v1/documents/get')
    assert(response.status_code == 400)
    assert(response.text == 'Parameter \'id\' not found')

# TODO
# list
# list empty
# update
# update partial
# update payload
# update missing
# update bad
# delete
# delete missing
# delete bad
# delete twice
