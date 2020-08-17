i2c������ƥ�����:

drivers/i2c/i2c-core.c 

static int i2c_device_match(struct device *dev, struct device_driver *drv)
{
	struct i2c_client	*client = i2c_verify_client(dev);
	struct i2c_driver	*driver;

	if (!client)
		return 0;

	/* Attempt an OF style match */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* Then ACPI style match */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	driver = to_i2c_driver(drv);
	/* match on an id table if there is one */
	if (driver->id_table)
		return i2c_match_id(driver->id_table, client) != NULL;

	return 0;
}


static int i2c_device_probe(struct device *dev)
{
	struct i2c_client	*client = i2c_verify_client(dev);
	struct i2c_driver	*driver;
	int status;

	if (!client)
		return 0;

	//��������1:���û��id_table��ֱ�Ӳ�����probe������!!!!!!!
	driver = to_i2c_driver(dev->driver);
	if (!driver->probe || !driver->id_table)
		return -ENODEV;
	
	//������һ������2:
	//ԭ�����ڵ���probe������ʱ��,�ٴ�ʹ����id_table�е����ֺ�client��¼�����ֽ�����ƥ��
	//�϶�:
	//����ʲôʱ���㶼������i2c_driver����дid_table��������ͻ��군
	status = driver->probe(client, i2c_match_id(driver->id_table, client));

	return status;
}

static const struct i2c_device_id *i2c_match_id(const struct i2c_device_id *id,
						const struct i2c_client *client)
{
	while (id->name[0]) {
		if (strcmp(client->name, id->name) == 0)
			return id;
		id++;
	}
	return NULL;
}
