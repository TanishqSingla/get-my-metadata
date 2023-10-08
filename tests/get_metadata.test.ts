import { CString, dlopen, suffix } from 'bun:ffi';
import { describe, expect, test } from 'bun:test';

const path = `../libget_metadata.${suffix}`;

const { symbols: { get_metadata } } = dlopen(path, {
	get_metadata: {
		args: ['cstring'],
		returns: 'cstring'
	}
});

function toCStringBuffer(str: string) {
	str += '\0'
	return Buffer.from(str);
}

describe("get_metadata", () => {
	test("returns with error message if file doesn't exists", () => {
		const result = get_metadata(toCStringBuffer('HelloWorld'));

		expect(String(result)).toBe("Error: file does not exist HelloWorld");
	});

	test("should get the json data from readme file", () => {
		const result = get_metadata(toCStringBuffer("README.md"));

		expect(JSON.parse(String(result))).toStrictEqual({title: 'test file'});
	});
});
