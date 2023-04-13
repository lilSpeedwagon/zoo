
import pytest

from zootest.mocks import MockAny

from .conftest import DocumentDbService


def _create_document(document_db: DocumentDbService, name: str = 'doc', owner: str = 'me',
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


def test_create(document_db: DocumentDbService):
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


def test_without_payload(document_db: DocumentDbService):
    document = {
        'name': 'doc',
        'owner': 'me',
        'namespace': '',
    }
    response = document_db.post('/api/v1/documents/create', body=document)
    assert(response.status_code == 400)


def test_create_empty_payload(document_db: DocumentDbService):
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


def test_create_twice(document_db: DocumentDbService):
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
    

def test_get(document_db: DocumentDbService):
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


def test_get_missing(document_db: DocumentDbService):
    response = document_db.get(f'/api/v1/documents/get?id=0')
    assert(response.status_code == 404)
    assert(response.text == 'Document with id=\'0\' not found')


def test_get_bad_request(document_db: DocumentDbService):
    response = document_db.get(f'/api/v1/documents/get')
    assert(response.status_code == 400)
    assert(response.text == 'Parameter \'id\' not found')


def test_list_empty(document_db: DocumentDbService):
    response = document_db.get(f'/api/v1/documents/list')
    assert(response.status_code == 200)
    assert(response.json() == {'items': []})


def test_list(document_db: DocumentDbService):
    documents = [
        _create_document(document_db, 'name1', 'owner1', 'namespace1', 'payload1'),
        _create_document(document_db, 'name2', 'owner2', 'namespace2', 'payload2'),
        _create_document(document_db, 'name3', 'owner3', 'namespace3', 'payload3'),
    ]
    
    response = document_db.get(f'/api/v1/documents/list')
    assert(response.status_code == 200)
    assert(response.json() == {'items': documents})


def test_update(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/update', body={
        'id': document['id'],
        'name': 'new name',
        'namespace': 'new namespace',
    })
    assert(response.status_code == 200)
    result = response.json()
    assert(result == {
        'id': document['id'],
        'created': document['created'],
        'updated': MockAny(),
        'name': 'new name',
        'owner': document['owner'],
        'namespace': 'new namespace',
    })


def test_update_payload(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/update', body={
        'id': document['id'],
        'payload': 'new payload',
    })
    assert(response.status_code == 200)
    result = response.json()
    assert(result == {
        'id': document['id'],
        'created': document['created'],
        'updated': MockAny(),
        'name': document['name'],
        'owner': document['owner'],
        'namespace': document['namespace'],
    })

    response = document_db.get(f'/api/v1/documents/get?id={document["id"]}')
    assert(response.status_code == 200)
    assert(response.json()['payload'] == 'new payload')


def test_update_missing(document_db: DocumentDbService):
    response = document_db.post(f'/api/v1/documents/update', 
                                body={'id': 0, 'name': 'new_name'})
    assert(response.status_code == 404)
    assert(response.text == 'Document with id=\'0\' not found')


def test_update_missing_id(document_db: DocumentDbService):
    response = document_db.post(f'/api/v1/documents/update',
                                body={'name': 'new name'})
    assert(response.status_code == 400)
    assert(response.text == 'Key \'id\' is required.')


def test_update_nothing_to_update(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/update',
                                body={'id': document['id']})
    assert(response.status_code == 200)
    assert(response.json() == document)


def test_update_extra_key(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/update', body={
        'id': document['id'],
        'name': 'new name',
        'extra_key': 'value'
    })
    assert(response.status_code == 200)
    assert(response.json() == {
        'id': document['id'],
        'created': MockAny(),
        'updated': MockAny(),
        'name': 'new name',
        'owner': document['owner'],
        'namespace': document['namespace'],
    })


def test_delete(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/delete',
                                body={'id': document['id']})
    assert(response.status_code == 200)
    assert(response.json() == document)


def test_delete_missing(document_db: DocumentDbService):
    response = document_db.post(f'/api/v1/documents/delete',
                                body={'id': 0})
    assert(response.status_code == 404)
    assert(response.text == 'Document with id=\'0\' not found')


def test_delete_missing_id(document_db: DocumentDbService):
    response = document_db.post(f'/api/v1/documents/delete', body={})
    assert(response.status_code == 400)
    assert(response.text == 'Key \'id\' is required.')


def test_delete_twice(document_db: DocumentDbService):
    document = _create_document(document_db)
    response = document_db.post(f'/api/v1/documents/delete',
                                body={'id': document['id']})
    assert(response.status_code == 200)
    assert(response.json() == document)
    
    response = document_db.post(f'/api/v1/documents/delete',
                                body={'id': document['id']})
    assert(response.status_code == 404)
    assert(response.text == f'Document with id=\'{document["id"]}\' not found')


def test_clear(document_db: DocumentDbService):
    documents = [_create_document(document_db) for i in range(3)]
    response = document_db.post('/api/v1/documents/clear')
    assert(response.status_code == 200)
    assert(response.json() == {'items_deleted': len(documents)})


def test_reload_from_fs(document_db: DocumentDbService):
    documents = [_create_document(document_db) for i in range(3)]
    
    # reset and make sure that documents were reloaded from FS
    reset_response = document_db.reset()
    assert reset_response.status_code == 200

    response = response = document_db.get(f'/api/v1/documents/list')
    assert response.status_code == 200
    assert response.json() == {'items': documents}
