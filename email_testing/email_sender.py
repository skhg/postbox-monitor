#!/usr/bin/env python

import os


from_email = os.environ['postbox_from_email']
to_emails = os.environ['postbox_to_emails'].split(';')
smtp_server = os.environ['postbox_smtp_server']
smtp_server_port = os.environ['postbox_smtp_server_port']
smtp_username = os.environ['postbox_smtp_username']
smtp_password = os.environ['postbox_smtp_password']

