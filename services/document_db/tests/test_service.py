
def test_ping(document_db):
    response = document_db.get('/ping')
    assert(response.status_code == 200)
    assert(response.text == 'OK')
