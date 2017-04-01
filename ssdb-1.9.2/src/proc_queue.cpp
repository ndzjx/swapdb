/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
/* queue */
#include "serv.h"
#include "net/proc.h"
#include "net/server.h"

int proc_qsize(NetworkServer *net, Link *link, const Request &req, Response *resp){
	SSDBServer *serv = (SSDBServer *)net->data;
	CHECK_NUM_PARAMS(2);

	uint64_t len = 0;
	int ret = serv->ssdb->LLen(req[1], &len);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		resp->reply_int(ret, len);
	}

	return 0;
}


int proc_qpush_frontx(NetworkServer *net, Link *link, const Request &req, Response *resp){
	CHECK_NUM_PARAMS(3);
	SSDBServer *serv = (SSDBServer *)net->data;

	const Bytes &name = req[1];
	uint64_t len = 0;
	int ret = serv->ssdb->LPushX(name, req, 2, &len);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		resp->reply_int(ret, len);
	}

	return 0;
}


int proc_qpush_front(NetworkServer *net, Link *link, const Request &req, Response *resp){
	CHECK_NUM_PARAMS(3);
	SSDBServer *serv = (SSDBServer *)net->data;

	const Bytes &name = req[1];
 	uint64_t len = 0;
	int ret = serv->ssdb->LPush(name, req, 2, &len);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		resp->reply_int(ret, len);
	}

	return 0;
}

int proc_qpush_backx(NetworkServer *net, Link *link, const Request &req, Response *resp){
	CHECK_NUM_PARAMS(3);
	SSDBServer *serv = (SSDBServer *)net->data;

	uint64_t len = 0;
	int ret = serv->ssdb->RPushX(req[1], req, 2, &len);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		resp->reply_int(ret, len);
	}

	return 0;
}


int proc_qpush_back(NetworkServer *net, Link *link, const Request &req, Response *resp){
    CHECK_NUM_PARAMS(3);
    SSDBServer *serv = (SSDBServer *)net->data;

    uint64_t len = 0;
    int ret = serv->ssdb->RPush(req[1], req, 2, &len);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		resp->reply_int(ret, len);
	}

	return 0;
}


int proc_qpop_front(NetworkServer *net, Link *link, const Request &req, Response *resp){
	CHECK_NUM_PARAMS(2);
	SSDBServer *serv = (SSDBServer *)net->data;

	const Bytes &name = req[1];

	std::pair<std::string, bool> val;
	int ret = serv->ssdb->LPop(name, val);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		if (val.second) {
			resp->reply_get(1, &val.first);
		} else {
			resp->reply_get(0, &val.first);
		};

	}

	return 0;
}

int proc_qpop_back(NetworkServer *net, Link *link, const Request &req, Response *resp){
    CHECK_NUM_PARAMS(2);
    SSDBServer *serv = (SSDBServer *)net->data;

	std::pair<std::string, bool> val;
    int ret = serv->ssdb->RPop(req[1], val);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		if (val.second) {
			resp->reply_get(1, &val.first);
		} else {
			resp->reply_get(0, &val.first);
		};

	}

	return 0;
}


int proc_qtrim(NetworkServer *net, Link *link, const Request &req, Response *resp){
	SSDBServer *serv = (SSDBServer *)net->data;
	CHECK_NUM_PARAMS(4);

	int64_t begin = req[2].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}

	int64_t end = req[3].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}


	int ret = serv->ssdb->ltrim(req[1], begin, end);
	check_key(ret);
	if (ret < 0){
		resp->resp.clear();
		reply_err_return(ret);
	} else {
		resp->reply_status(ret);
	}

	return 0;
}



int proc_qslice(NetworkServer *net, Link *link, const Request &req, Response *resp){
	SSDBServer *serv = (SSDBServer *)net->data;
	CHECK_NUM_PARAMS(4);

	int64_t begin = req[2].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}

	int64_t end = req[3].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}

	resp->reply_list_ready();
	int ret = serv->ssdb->lrange(req[1], begin, end, resp->resp);
	check_key(ret);
	if (ret < 0){
		resp->resp.clear();
		reply_err_return(ret);
	}

	return 0;
}

int proc_qget(NetworkServer *net, Link *link, const Request &req, Response *resp){
	SSDBServer *serv = (SSDBServer *)net->data;
	CHECK_NUM_PARAMS(3);

	int64_t index = req[2].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}

	std::pair<std::string, bool> val;
	int ret = serv->ssdb->LIndex(req[1], index, val);
	check_key(ret);
    if (ret < 0) {
		reply_err_return(ret);
	} else {
		if (val.second) {
			resp->reply_get(1, &val.first);
		} else {
			resp->reply_get(0, &val.first);
		}
	}

	return 0;
}

int proc_qset(NetworkServer *net, Link *link, const Request &req, Response *resp){
	SSDBServer *serv = (SSDBServer *)net->data;
	CHECK_NUM_PARAMS(4);

	const Bytes &name = req[1];
	int64_t index = req[2].Int64();
	if (errno == EINVAL){
		reply_err_return(INVALID_INT);
	}

	const Bytes &item = req[3];
	int ret = serv->ssdb->LSet(name, index, item);
	check_key(ret);
	if(ret < 0){
		reply_err_return(ret);
	} else if ( ret == 0) {
		//???
		reply_errinfo_return("ERR no such key");
	} else{
		//TODO CHECK HERE
		resp->reply_ok();
	}
	return 0;
}
