/*
 * clustering.h
 *
 * Copyright (C) 2016 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 * Aerospike cluster formation v5 based on paxos.
 * Complete discussion of the algorithm can be found
 * https://docs.google.com/document/d/1u-27aeZD9no9wiWgt1_BsTSg_6ewG9VBI2sYA0g01BE/edit#
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "citrusleaf/cf_vector.h"

#include "fabric/hlc.h"

/*
 * ----------------------------------------------------------------------------
 * Public data structures.
 * ----------------------------------------------------------------------------
 */
/**
 * Aerospike cluster key.
 */
typedef uint64_t as_cluster_key;

/**
 * Aerospike clustering protocol identifier.
 */
typedef uint32_t as_cluster_proto_identifier;

/**
 * Configuration for the clustering algorithm.
 */
typedef struct as_clustering_config_s
{
	/**
	 * The smallest allowed  cluster size.
	 */
	uint32_t cluster_size_min;

	/**
	 * Indicates if clique based eviction is enabled.
	 */
	bool clique_based_eviction_enabled;

	/**
	 * Current protocol identifier.
	 */
	as_cluster_proto_identifier protocol_identifier;

} as_clustering_config;

/**
 * The clustering protocol versions.
 */
typedef enum as_clustering_protocol_version
{
	AS_CLUSTERING_PROTOCOL_UNDEF,
	AS_CLUSTERING_PROTOCOL_NONE,
	AS_CLUSTERING_PROTOCOL_V1,
	AS_CLUSTERING_PROTOCOL_V2,
	AS_CLUSTERING_PROTOCOL_V3,
	AS_CLUSTERING_PROTOCOL_V4,
	AS_CLUSTERING_PROTOCOL_V5
} as_clustering_protocol_version;

/**
 * Clustering event type.
 */
typedef enum as_clustering_event_type_e
{
	/**
	 * Cluster membership for this node changed.
	 */
	AS_CLUSTERING_CLUSTER_CHANGED,

	/**
	 * This node became an orphan node.
	 */
	AS_CLUSTERING_ORPHANED
} as_clustering_event_type;

/**
 * Clustering event.
 */
typedef struct as_clustering_event_s
{
	/**
	 * The clustering event type.
	 */
	as_clustering_event_type type;

	/**
	 * The cluster key. Will be non-zero if this is a cluster change event.
	 */
	as_cluster_key cluster_key;

	/**
	 * The new succession list. It will not be empty if this is a cluster change
	 * event.
	 *
	 * The allocated space will be freed once the event processing is complete.
	 * Listeners should always create a copy of this list, if it needs to be
	 * used later on by the listener.
	 */
	cf_vector* succession_list;
} as_clustering_event;

/*
 * ----------------------------------------------------------------------------
 * Public API.
 * ----------------------------------------------------------------------------
 */
/**
 * Initialize clustering subsystem.
 */
void
as_clustering_init();

/**
 * Start clustering subsystem.
 */
void
as_clustering_start();

/**
 * Stop clustering subsystem.
 */
void
as_clustering_stop();

/**
 * Return the quantum interval, i.e., the interval at which cluster change
 * decisions are taken. The unit is milliseconds.
 */
uint32_t
as_clustering_quantum_interval();

/*
 * ---- Clustering info command functions. ----
 */
/**
 * If false means than either this node is orphaned, or is undergoing a cluster
 * change.
 */
bool
as_clustering_has_integrity();

/**
 * Indicates if self node is orphaned.
 */
bool
as_clustering_is_orphan();

/**
 * Dump clustering state to the log.
 */
void
as_clustering_dump(bool verbose);

/**
 * Set the min cluster size.
 */
int
as_clustering_cluster_size_min_set(uint32_t new_cluster_size_min);
